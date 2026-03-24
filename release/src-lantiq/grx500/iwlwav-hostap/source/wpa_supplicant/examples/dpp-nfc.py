#!/usr/bin/python3
#
# Example nfcpy to wpa_supplicant wrapper for DPP NFC operations
# Copyright (c) 2012-2013, Jouni Malinen <j@w1.fi>
# Copyright (c) 2019-2020, The Linux Foundation
#
# This software may be distributed under the terms of the BSD license.
# See README for more details.

import os
import sys
import time
import threading
import argparse

import nfc
import ndef

import logging

scriptsdir = os.path.dirname(os.path.realpath("dpp-nfc.py"))
sys.path.append(os.path.join(scriptsdir, '..', '..', 'wpaspy'))
import wpaspy

wpas_ctrl = '/var/run/wpa_supplicant'
ifname = None
init_on_touch = False
in_raw_mode = False
prev_tcgetattr = 0
no_input = False
srv = None
continue_loop = True
terminate_now = False
summary_file = None
success_file = None

def summary(txt):
    print(txt)
    if summary_file:
        with open(summary_file, 'a') as f:
            f.write(txt + "\n")

def success_report(txt):
    summary(txt)
    if success_file:
        with open(success_file, 'a') as f:
            f.write(txt + "\n")

def wpas_connect():
    ifaces = []
    if os.path.isdir(wpas_ctrl):
        try:
            ifaces = [os.path.join(wpas_ctrl, i) for i in os.listdir(wpas_ctrl)]
        except OSError as error:
            print("Could not find wpa_supplicant: ", error)
            return None

    if len(ifaces) < 1:
        print("No wpa_supplicant control interface found")
        return None

    for ctrl in ifaces:
        if ifname and ifname not in ctrl:
            continue
        if os.path.basename(ctrl).startswith("p2p-dev-"):
            # skip P2P management interface
            continue
        try:
            print("Trying to use control interface " + ctrl)
            wpas = wpaspy.Ctrl(ctrl)
            return wpas
        except Exception as e:
            pass
    summary("Could not connect to wpa_supplicant")
    return None

def dpp_nfc_uri_process(uri):
    wpas = wpas_connect()
    if wpas is None:
        return False
    peer_id = wpas.request("DPP_NFC_URI " + uri)
    if "FAIL" in peer_id:
        print("Could not parse DPP URI from NFC URI record")
        return False
    peer_id = int(peer_id)
    print("peer_id=%d" % peer_id)
    cmd = "DPP_AUTH_INIT peer=%d" % peer_id
    res = wpas.request(cmd)
    if "OK" not in res:
        print("Failed to initiate DPP Authentication")
        return False
    print("DPP Authentication initiated")
    return True

def dpp_hs_tag_read(record):
    wpas = wpas_connect()
    if wpas is None:
        return False
    print(record)
    if len(record.data) < 5:
        print("Too short DPP HS")
        return False
    if record.data[0] != 0:
        print("Unexpected URI Identifier Code")
        return False
    uribuf = record.data[1:]
    try:
        uri = uribuf.decode()
    except:
        print("Invalid URI payload")
        return False
    print("URI: " + uri)
    if not uri.startswith("DPP:"):
        print("Not a DPP URI")
        return False
    return dpp_nfc_uri_process(uri)

def get_status(wpas, extra=None):
    if extra:
        extra = "-" + extra
    else:
        extra = ""
    res = wpas.request("STATUS" + extra)
    lines = res.splitlines()
    vals = dict()
    for l in lines:
        try:
            [name, value] = l.split('=', 1)
        except ValueError:
            logger.info("Ignore unexpected status line: " + l)
            continue
        vals[name] = value
    return vals

def get_status_field(wpas, field, extra=None):
    vals = get_status(wpas, extra)
    if field in vals:
        return vals[field]
    return None

def own_addr(wpas):
    return get_status_field(wpas, "address")

def dpp_bootstrap_gen(wpas, type="qrcode", chan=None, mac=None, info=None,
                      curve=None, key=None):
    cmd = "DPP_BOOTSTRAP_GEN type=" + type
    if chan:
        cmd += " chan=" + chan
    if mac:
        if mac is True:
            mac = own_addr(wpas)
        cmd += " mac=" + mac.replace(':', '')
    if info:
        cmd += " info=" + info
    if curve:
        cmd += " curve=" + curve
    if key:
        cmd += " key=" + key
    res = wpas.request(cmd)
    if "FAIL" in res:
        raise Exception("Failed to generate bootstrapping info")
    return int(res)

def dpp_start_listen(wpas, freq):
    if get_status_field(wpas, "bssid[0]"):
        summary("Own AP freq: %s MHz" % str(get_status_field(wpas, "freq")))
        if get_status_field(wpas, "beacon_set", extra="DRIVER") is None:
            summary("Enable beaconing to have radio ready for RX")
            wpas.request("DISABLE")
            wpas.request("SET start_disabled 0")
            wpas.request("ENABLE")
    cmd = "DPP_LISTEN %d" % freq
    global enrollee_only
    global configurator_only
    if enrollee_only:
        cmd += " role=enrollee"
    elif configurator_only:
        cmd += " role=configurator"
    global netrole
    if netrole:
        cmd += " netrole=" + netrole
    summary(cmd)
    res = wpas.request(cmd)
    if "OK" not in res:
        summary("Failed to start DPP listen", color=C_RED)
        return False
    return True

def wpas_get_nfc_uri(start_listen=True, pick_channel=False, chan_override=None):
    listen_freq = 2412
    wpas = wpas_connect()
    if wpas is None:
        return None
    global own_id, chanlist
    own_id = dpp_bootstrap_gen(wpas, type="nfc-uri", chan=chanlist, mac=True)
    res = wpas.request("DPP_BOOTSTRAP_GET_URI %d" % own_id).rstrip()
    if "FAIL" in res:
        return None
    if start_listen:
        if not dpp_start_listen(wpas, listen_freq):
            raise Exception("Failed to start listen operation on %d MHz" % listen_freq)
    return res

def wpas_report_handover_req(uri):
    wpas = wpas_connect()
    if wpas is None:
        return None
    global own_id
    cmd = "DPP_NFC_HANDOVER_REQ own=%d uri=%s" % (own_id, uri)
    return wpas.request(cmd)

def wpas_report_handover_sel(uri):
    wpas = wpas_connect()
    if wpas is None:
        return None
    global own_id
    cmd = "DPP_NFC_HANDOVER_SEL own=%d uri=%s" % (own_id, uri)
    return wpas.request(cmd)

def dpp_handover_client(handover, alt=False):
    summary("About to start run_dpp_handover_client (alt=%s)" % str(alt))
    if alt:
        handover.i_m_selector = False
    run_dpp_handover_client(handover, alt)
    summary("Done run_dpp_handover_client (alt=%s)" % str(alt))

def run_client_alt(handover, alt):
    if handover.start_client_alt and not alt:
        handover.start_client_alt = False
        summary("Try to send alternative handover request")
        dpp_handover_client(handover, alt=True)

class HandoverClient(nfc.handover.HandoverClient):
    def __init__(self, handover, llc):
        super(HandoverClient, self).__init__(llc)
        self.handover = handover

    def recv_records(self, timeout=None):
        msg = self.recv_octets(timeout)
        if msg is None:
            return None
        records = list(ndef.message_decoder(msg, 'relax'))
        if records and records[0].type == 'urn:nfc:wkt:Hs':
            summary("Handover client received message '{0}'".format(records[0].type))
            return list(ndef.message_decoder(msg, 'relax'))
        summary("Handover client received invalid message: %s" + binascii.hexlify(msg))
        return None

    def recv_octets(self, timeout=None):
        start = time.time()
        msg = bytearray()
        while True:
            poll_timeout = 0.1 if timeout is None or timeout > 0.1 else timeout
            if self.socket.poll('recv', poll_timeout) is None:
                if timeout:
                    timeout -= time.time() - start
                    if timeout <= 0:
                        return None
                    start = time.time()
                continue
            try:
                r = self.socket.recv()
                if r is None:
                    return None
                msg += r
            except TypeError:
                return b''
            try:
                list(ndef.message_decoder(msg, 'strict', {}))
                return bytes(msg)
            except ndef.DecodeError:
                if timeout:
                    timeout -= time.time() - start
                    if timeout <= 0:
                        return None
                    start = time.time()
                continue
        return None

def run_dpp_handover_client(handover, alt=False):
    chan_override = None
    if alt:
        chan_override = handover.altchanlist
        handover.alt_proposal_used = True
    global test_uri, test_alt_uri
    if test_uri:
        summary("TEST MODE: Using specified URI (alt=%s)" % str(alt))
        uri = test_alt_uri if alt else test_uri
    else:
        uri = wpas_get_nfc_uri(start_listen=False, chan_override=chan_override)
    if uri is None:
        summary("Cannot start handover client - no bootstrap URI available",
                color=C_RED)
        return
    handover.my_uri = uri
    uri = ndef.UriRecord(uri)
    print("NFC URI record for DPP: " + str(uri))
    carrier = ndef.Record('application/vnd.wfa.dpp', 'A', uri.data)
    hr = ndef.HandoverRequestRecord(version="1.4", crn=os.urandom(2))
    hr.add_alternative_carrier('active', carrier.name)
    message = [hr, carrier]
    print("NFC Handover Request message for DPP: " + str(message))

    client = nfc.handover.HandoverClient(llc)
    try:
        summary("Trying to initiate NFC connection handover")
        client.connect()
        summary("Connected for handover")
    except nfc.llcp.ConnectRefused:
        summary("Handover connection refused")
        client.close()
        return
    except Exception as e:
        summary("Other exception: " + str(e))
        client.close()
        return

    summary("Sending handover request")

    if not client.send_records(message):
        summary("Failed to send handover request")
        client.close()
        return

    summary("Receiving handover response")
    try:
        start = time.time()
        message = client.recv_records(timeout=3.0)
        end = time.time()
        summary("Received {} record(s) in {} seconds".format(len(message) if message is not None else -1, end - start))
    except Exception as e:
        # This is fine if we are the handover selector
        if handover.hs_sent:
            summary("Client receive failed as expected since I'm the handover server: %s" % str(e))
        elif handover.alt_proposal_used and not alt:
            summary("Client received failed for initial proposal as expected since alternative proposal was also used: %s" % str(e))
        else:
            summary("Client receive failed: %s" % str(e), color=C_RED)
        message = None
    if message is None:
        summary("No response received")
        client.close()
        return
    print("Received message: " + str(message))
    if len(message) < 1 or \
       not isinstance(message[0], ndef.HandoverSelectRecord):
        summary("Response was not Hs - received: " + message.type)
        client.close()
        return

    print("Received message")
    print("alternative carriers: " + str(message[0].alternative_carriers))

    dpp_found = False
    for carrier in message:
        if isinstance(carrier, ndef.HandoverSelectRecord):
            continue
        print("Remote carrier type: " + carrier.type)
        if carrier.type == "application/vnd.wfa.dpp":
            if len(carrier.data) == 0 or carrier.data[0] != 0:
                print("URI Identifier Code 'None' not seen")
                continue
            print("DPP carrier type match - send to wpa_supplicant")
            dpp_found = True
            uri = carrier.data[1:].decode("utf-8")
            print("DPP URI: " + uri)
            res = wpas_report_handover_sel(uri)
            if res is None or "FAIL" in res:
                summary("DPP handover report rejected")
                break

            success_report("DPP handover reported successfully (initiator)")
            print("peer_id=" + res)
            peer_id = int(res)
            wpas = wpas_connect()
            if wpas is None:
                break

            global enrollee_only
            global config_params
            if enrollee_only:
                extra = " role=enrollee"
            elif config_params:
                extra = " role=configurator " + config_params
            else:
                # TODO: Single Configurator instance
                res = wpas.request("DPP_CONFIGURATOR_ADD")
                if "FAIL" in res:
                    print("Failed to initiate Configurator")
                    break
                conf_id = int(res)
                extra = " conf=sta-dpp configurator=%d" % conf_id
            global own_id
            print("Initiate DPP authentication")
            cmd = "DPP_AUTH_INIT peer=%d own=%d" % (peer_id, own_id)
            cmd += extra
            res = wpas.request(cmd)
            if "FAIL" in res:
                print("Failed to initiate DPP authentication")
            break

    if not dpp_found:
        print("DPP carrier not seen in response - allow peer to initiate a new handover with different parameters")
        client.close()
        print("Returning from dpp_handover_client")
        return

    print("Remove peer")
    client.close()
    print("Done with handover")
    global only_one
    if only_one:
        print("only_one -> stop loop")
        global continue_loop
        continue_loop = False

    global no_wait
    if no_wait:
        print("Trying to exit..")
        global terminate_now
        terminate_now = True

    print("Returning from dpp_handover_client")

class HandoverServer(nfc.handover.HandoverServer):
    def __init__(self, llc):
        super(HandoverServer, self).__init__(llc)
        self.sent_carrier = None
        self.ho_server_processing = False
        self.success = False
        self.try_own = False

    def process_handover_request_message(self, records):
        self.ho_server_processing = True
        clear_raw_mode()
        print("\nHandoverServer - request received: " + str(records))

        carrier = None
        hs = ndef.HandoverSelectRecord('1.4')
        sel = [hs]

        found = False

        for carrier in records:
            if isinstance(carrier, ndef.HandoverRequestRecord):
                continue
            print("Remote carrier type: " + carrier.type)
            if carrier.type == "application/vnd.wfa.dpp":
                print("DPP carrier type match - add DPP carrier record")
                if len(carrier.data) == 0 or carrier.data[0] != 0:
                    print("URI Identifier Code 'None' not seen")
                    continue
                uri = carrier.data[1:].decode("utf-8")
                print("Received DPP URI: " + uri)

                data = wpas_get_nfc_uri(start_listen=False)
                print("Own URI (pre-processing): %s" % data)

                if test_uri:
                    summary("TEST MODE: Fake processing")
                    res = "OK"
                else:
                    res = wpas_report_handover_req(uri)
                if res is None or "FAIL" in res:
                    print("DPP handover request processing failed")
                    continue

                if test_alt_uri:
                    summary("TEST MODE: Reject initial proposal")
                    continue

                found = True
                self.received_carrier = carrier

                wpas = wpas_connect()
                if wpas is None:
                    continue
                global own_id
                data = wpas.request("DPP_BOOTSTRAP_GET_URI %d" % own_id).rstrip()
                if "FAIL" in data:
                    continue
                print("Own URI (post-processing): %s" % data)
                uri = ndef.UriRecord(data)
                summary("Own bootstrapping NFC URI record: " + str(uri))

                if not test_uri:
                    info = wpas.request("DPP_BOOTSTRAP_INFO %d" % own_id)
                    freq = None
                    for line in info.splitlines():
                        if line.startswith("use_freq="):
                            freq = int(line.split('=')[1])
                    if freq is None or freq == 0:
                        summary("No channel negotiated over NFC - use channel 6")
                        freq = 2437
                    else:
                        summary("Negotiated channel: %d MHz" % freq)
                    if not dpp_start_listen(wpas, freq):
                        break

                carrier = ndef.Record('application/vnd.wfa.dpp', 'A', uri.data)
                print("Own DPP carrier record: " + str(carrier))
                hs.add_alternative_carrier('active', carrier.name)
                sel = [hs, carrier]
                break

        summary("Sending handover select: " + str(sel))
        if found:
            self.success = True
        else:
            self.try_own = True
        return sel

def clear_raw_mode():
    import sys, tty, termios
    global prev_tcgetattr, in_raw_mode
    if not in_raw_mode:
        return
    fd = sys.stdin.fileno()
    termios.tcsetattr(fd, termios.TCSADRAIN, prev_tcgetattr)
    in_raw_mode = False

def getch():
    import sys, tty, termios, select
    global prev_tcgetattr, in_raw_mode
    fd = sys.stdin.fileno()
    prev_tcgetattr = termios.tcgetattr(fd)
    ch = None
    try:
        tty.setraw(fd)
        in_raw_mode = True
        [i, o, e] = select.select([fd], [], [], 0.05)
        if i:
            ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, prev_tcgetattr)
        in_raw_mode = False
    return ch

def dpp_tag_read(tag):
    success = False
    for record in tag.ndef.records:
        print(record)
        print("record type " + record.type)
        if record.type == "application/vnd.wfa.dpp":
            summary("DPP HS tag - send to wpa_supplicant")
            success = dpp_hs_tag_read(record)
            break
        if isinstance(record, ndef.UriRecord):
            print("URI record: uri=" + record.uri)
            print("URI record: iri=" + record.iri)
            if record.iri.startswith("DPP:"):
                print("DPP URI")
                if not dpp_nfc_uri_process(record.iri):
                    break
                success = True
            else:
                print("Ignore unknown URI")
            break

    if success:
        success_report("Tag read succeeded")

    return success

def rdwr_connected_write_tag(tag):
    summary("Tag found - writing - " + str(tag))
    if not tag.ndef.is_writeable:
        print("Not a writable tag")
        return
    global dpp_tag_data
    if tag.ndef.capacity < len(dpp_tag_data):
        print("Not enough room for the message")
        return
    tag.ndef.records = dpp_tag_data
    success_report("Tag write succeeded")
    print("Done - remove tag")
    global only_one
    if only_one:
        global continue_loop
        continue_loop = False
    global dpp_sel_wait_remove
    return dpp_sel_wait_remove

def write_nfc_uri(clf, wait_remove=True):
    print("Write NFC URI record")
    data = wpas_get_nfc_uri()
    if data is None:
        summary("Could not get NFC URI from wpa_supplicant")
        return

    global dpp_sel_wait_remove
    dpp_sel_wait_remove = wait_remove
    print("URI: %s" % data)
    uri = ndef.UriRecord(data)
    print(uri)

    print("Touch an NFC tag")
    global dpp_tag_data
    dpp_tag_data = [uri]
    clf.connect(rdwr={'on-connect': rdwr_connected_write_tag})

def write_nfc_hs(clf, wait_remove=True):
    print("Write NFC Handover Select record on a tag")
    data = wpas_get_nfc_uri()
    if data is None:
        summary("Could not get NFC URI from wpa_supplicant")
        return

    global dpp_sel_wait_remove
    dpp_sel_wait_remove = wait_remove
    print("URI: %s" % data)
    uri = ndef.UriRecord(data)
    print(uri)
    carrier = ndef.Record('application/vnd.wfa.dpp', 'A', uri.data)
    hs = ndef.HandoverSelectRecord('1.4')
    hs.add_alternative_carrier('active', carrier.name)
    print(hs)
    print(carrier)

    print("Touch an NFC tag")
    global dpp_tag_data
    dpp_tag_data = [hs, carrier]
    print(dpp_tag_data)
    clf.connect(rdwr={'on-connect': rdwr_connected_write_tag})

def rdwr_connected(tag):
    global only_one, no_wait
    summary("Tag connected: " + str(tag))

    if tag.ndef:
        print("NDEF tag: " + tag.type)
        print(tag.ndef.records)
        success = dpp_tag_read(tag)
        if only_one and success:
            global continue_loop
            continue_loop = False
    else:
        summary("Not an NDEF tag - remove tag")
        return True

    return not no_wait

def llcp_worker(llc):
    global init_on_touch
    if init_on_touch:
        print("Starting handover client")
        dpp_handover_client(llc)
        print("Exiting llcp_worker thread (init_in_touch)")
        return

    global no_input
    if no_input:
        print("Wait for handover to complete")
    else:
        print("Wait for handover to complete - press 'i' to initiate")
    global srv
    global wait_connection
    while not wait_connection and srv.sent_carrier is None:
        if srv.try_own:
            srv.try_own = False
            print("Try to initiate another handover with own parameters")
            dpp_handover_client(llc)
            print("Exiting llcp_worker thread (retry with own parameters)")
            return
        if srv.ho_server_processing:
            time.sleep(0.025)
        elif no_input:
            time.sleep(0.5)
        else:
            res = getch()
            if res != 'i':
                continue
            clear_raw_mode()
            print("Starting handover client")
            dpp_handover_client(llc)
            print("Exiting llcp_worker thread (manual init)")
            return

    clear_raw_mode()
    print("\rExiting llcp_worker thread")

def llcp_startup(llc):
    print("Start LLCP server")
    global srv
    srv = HandoverServer(llc)
    return llc

def llcp_connected(llc):
    print("P2P LLCP connected")
    global wait_connection
    wait_connection = False
    global init_on_touch
    if not init_on_touch:
        global srv
        srv.start()
    if init_on_touch or not no_input:
        threading.Thread(target=llcp_worker, args=(llc,)).start()
    return True

def llcp_release(llc):
    print("LLCP release")
    return True

def terminate_loop():
    global terminate_now
    return terminate_now

def main():
    clf = nfc.ContactlessFrontend()

    parser = argparse.ArgumentParser(description='nfcpy to wpa_supplicant integration for DPP NFC operations')
    parser.add_argument('-d', const=logging.DEBUG, default=logging.INFO,
                        action='store_const', dest='loglevel',
                        help='verbose debug output')
    parser.add_argument('-q', const=logging.WARNING, action='store_const',
                        dest='loglevel', help='be quiet')
    parser.add_argument('--only-one', '-1', action='store_true',
                        help='run only one operation and exit')
    parser.add_argument('--init-on-touch', '-I', action='store_true',
                        help='initiate handover on touch')
    parser.add_argument('--no-wait', action='store_true',
                        help='do not wait for tag to be removed before exiting')
    parser.add_argument('--ifname', '-i',
                        help='network interface name')
    parser.add_argument('--no-input', '-a', action='store_true',
                        help='do not use stdout input to initiate handover')
    parser.add_argument('--tag-read-only', '-t', action='store_true',
                        help='tag read only (do not allow connection handover)')
    parser.add_argument('--handover-only', action='store_true',
                        help='connection handover only (do not allow tag read)')
    parser.add_argument('--enrollee', action='store_true',
                        help='run as Enrollee-only')
    parser.add_argument('--configurator', action='store_true',
                        help='run as Configurator-only')
    parser.add_argument('--config-params', default='',
                        help='configurator parameters')
    parser.add_argument('--summary',
                        help='summary file for writing status updates')
    parser.add_argument('--success',
                        help='success file for writing success update')
    parser.add_argument('--device', default='usb', help='NFC device to open')
    parser.add_argument('--chan', default=None, help='channel list')
    parser.add_argument('--altchan', default=None, help='alternative channel list')
    parser.add_argument('--netrole', default=None, help='netrole for Enrollee')
    parser.add_argument('--test-uri', default=None,
                        help='test mode: initial URI')
    parser.add_argument('--test-alt-uri', default=None,
                        help='test mode: alternative URI')
    parser.add_argument('--test-sel-uri', default=None,
                        help='test mode: handover select URI')
    parser.add_argument('--test-crn', default=None,
                        help='test mode: hardcoded crn')
    parser.add_argument('command', choices=['write-nfc-uri',
                                            'write-nfc-hs'],
                        nargs='?')
    args = parser.parse_args()
    print(args)

    global only_one
    only_one = args.only_one

    global no_wait
    no_wait = args.no_wait

    global chanlist, altchanlist, netrole, test_uri, test_alt_uri, test_sel_uri
    global test_crn
    chanlist = args.chan
    altchanlist = args.altchan
    netrole = args.netrole
    test_uri = args.test_uri
    test_alt_uri = args.test_alt_uri
    test_sel_uri = args.test_sel_uri
    if args.test_crn:
        test_crn = struct.pack('>H', int(args.test_crn))
    else:
        test_crn = None

    logging.basicConfig(level=args.loglevel)

    global init_on_touch
    init_on_touch = args.init_on_touch

    global enrollee_only
    enrollee_only = args.enrollee

    global configurator_only
    configurator_only = args.configurator

    global config_params
    config_params = args.config_params

    if args.ifname:
        global ifname
        ifname = args.ifname
        print("Selected ifname " + ifname)

    if args.summary:
        global summary_file
        summary_file = args.summary

    if args.success:
        global success_file
        success_file = args.success

    if args.no_input:
        global no_input
        no_input = True

    clf = nfc.ContactlessFrontend()
    global wait_connection

    try:
        if not clf.open(args.device):
            print("Could not open connection with an NFC device")
            raise SystemExit

        if args.command == "write-nfc-uri":
            write_nfc_uri(clf, wait_remove=not args.no_wait)
            raise SystemExit

        if args.command == "write-nfc-hs":
            write_nfc_hs(clf, wait_remove=not args.no_wait)
            raise SystemExit

        global continue_loop
        while continue_loop:
            clear_raw_mode()
            print("\rWaiting for a tag or peer to be touched")
            wait_connection = True
            try:
                if args.tag_read_only:
                    if not clf.connect(rdwr={'on-connect': rdwr_connected}):
                        break
                elif args.handover_only:
                    if not clf.connect(llcp={'on-startup': llcp_startup,
                                             'on-connect': llcp_connected,
                                             'on-release': llcp_release},
                                       terminate=terminate_loop):
                        break
                else:
                    if not clf.connect(rdwr={'on-connect': rdwr_connected},
                                       llcp={'on-startup': llcp_startup,
                                             'on-connect': llcp_connected,
                                             'on-release': llcp_release},
                                       terminate=terminate_loop):
                        break
            except Exception as e:
                print("clf.connect failed: " + str(e))
                break

            global srv
            if only_one and srv and srv.success:
                raise SystemExit

    except KeyboardInterrupt:
        raise SystemExit
    finally:
        clf.close()

    raise SystemExit

if __name__ == '__main__':
    main()

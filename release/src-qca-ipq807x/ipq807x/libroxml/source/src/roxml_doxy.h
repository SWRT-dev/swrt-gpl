/** \file roxml_doxy.h
 *  \brief doxygen source documentation for libroxml.so
 *
 * This is the header file used to develop some
 * softwares using the libroxml.so library.
 * \author blunderer <blunderer@blunderer.org>
 * \date 23 Dec 2008
 *
 * Copyright (C) 2009 blunderer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * The author added a static linking exception, see License.txt.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/** \mainpage libroxml homepage
 *
 * \section intro_sec <introduction>
 * This library is minimum, easy-to-use, C implementation for XML file parsing
 * It includes:
 * <ul>
 * <li>the library \b libroxml for XML parsing inside applications.</li>
 * <li>the binary \b roxml, an xpath resolver that can be used from shell.</li>
 * <li>the module \b fuse.XML that can be used to mount an XML file as a filesystem.</li>
 * <li>the full \ref public-api and \ref priv-api documentation.</li>
 * </ul>
 *
 * The stripped binary is about 50K. The public API defines less than 30 functions to do all you need, making it very easy to start with.
 * For bug reporting, you can check the github dedicated project: https://github.com/blunderer/libroxml
 *
 * Libroxml is distributed under the terms of the <a href="http://www.gnu.org/licenses/lgpl-2.1.html">GNU Lesser General Public License v2.1</a> with a static linking exception (see <a href="https://github.com/blunderer/libroxml/blob/master/License.txt">License.txt</a>).
 * 
 * \section intro_end </introduction>
 * \section dl_sec <downloads>
 * The source code is available from github project GIT repository: 
 * \verbatim $> git clone https://github.com/blunderer/libroxml.git \endverbatim
 *
 * You can download source packages:
 * <ul>
 * <li>Lastest version is 3.0.2: <a href="http://download.libroxml.net/pool/v2.x/libroxml-3.0.2.tar.gz">download now</a></li>
 * <li>Go to the download section if you need older libroxml revision: http://download.libroxml.net </li>
 * </ul>
 * 
 * \section dl_end </downloads>
 * \section what_sec <why libroxml>
 * Because XML parsing is always hard to reinvent, and because very often XML lib are 
 * too big to fit with very little application. Libroxml target mainly embedded software and environment, but you can use it whenever you need to deal with XML since libroxml is ligth and fast. 
 * libroxml is now provided in buildroot (http://buildroot.uclibc.org).
 *
 * libroxml allow you to easily:
 * <ul>
 * <li> load / unload XML document from buffers or files.</li>
 * <li> navigate throughout an XML tree using simple getter API.</li>
 * <li> handle namespace.</li>
 * <li> use xpath syntax to access some nodes in the XML tree.</li>
 * <li> read nodes contents (text, attributes, comments ...)</li>
 * <li> create/modify XML trees and save them to a file or buffer.</li>
 *</ul>
 *
 * \note libroxml work with both strict XML documents but also with XML like formatted documents (without any <?XML?> definition...)
 * \warning libroxml may behave strangely when dealing with node names or attribute values bigger than 512 bytes. For those who really need it, they can increase this limit by modifying the ROXML_BASE_LEN define in roxml_defines.h.
 * \warning libroxml do not handle DOCTYPE nodes. However, it will nicely ignore them during parsing and will still return the XML tree.
 * \warning libroxml is not thread safe for all write operations (roxml_set_ns, roxml_add_node, roxml_del_node) therefore, those functions must be granted exclusive access to the XML tree when called. On the other hand, all other functions can be safely called simultaneously.
 *
 * \section what_end </why libroxml>
 * \section how_sec <how does it work>
 * You can refer to \ref public-api for documentation on all functions
 *
 * there are several groups of functions in public API: 
 * <ul>
 * <li> Load and close XML data from different sources.</li>
 * <li> Get nodes and navigate into the loaded XML tree.</li>
 * <li> Get the XML content of nodes, attributes...</li>
 * <li> Create and delete nodes, export tree.</li>
 * <li> Some libroxml specific functions for memory handling</li>
 * </ul>
 * \section how_end </how does it work>
 *
 * \section build_sec <building libroxml>
 * libroxml is a cross-platform library. It is developed on Linux OS, but is pretty much standalone and only depends of pthread mutexes in case it is compiled with thread safety enabled.
 * This should make the library work on any platform that is POSIX compliant.
 * Libroxml also works on Microsoft, the pthread API being converted to CriticalSection using basic macros.
 *
 * Libroxml build system relies on autotool to guaranty the best compatibility on all machines. A CMakeList.txt is also provided in order to ease the native build on Windows machines, but this one provides less options for tuning the behavior of library.
 *
 * Libroxml can be tuned to make it full-featured or minimalistic, in order to fit most usage:
 * <h2>Development options</h2>
 * <ul>
 * <li> --enable-docs           Enable building documentation.
 * <li> --enable-tests          Enable building unit tests (only available from git repository).
 * <li> --enable-debug          Turn on debugging.
 * </ul>
 * <h2>Feature options</h2>
 * <ul>
 * <li> --disable-file          Remove support for parsing a file, only parses char buffer containing the XML stream.
 * <li> --disable-edit          Disable the edition of the XML tree: read only XML.
 * <li> --disable-commit        Disable the ability to serialize the modified XML tree back to the disk or to a buffer.
 * <li> --disable-xpath		Disable support for Xpath handling
 * <li> --disable-roxml         Disable building roxml binary (xpath shell solver).
 * <li> --enable-verbose        Enable verbose parsing on errors (printf on stderr of parsing error as they come).
 * </ul>
 * <h2>Fine tuning for small systems</h2>
 * <ul>
 * <li> --disable-float		Remove support for float values in xpath comparisons (if any float is still parsed, it will be floor-ed)
 * <li> --disable-threadsafe    Make the library not threadsafe (but remove dependency on mutex).
 * <li> --enable-heapbuffers    Force all buffers to be allocated on the heap rather than on the stack for tiny systems with small stacks.
 * <li> --enable-smallbuffer    Force using smaller buffer for all XML operations. Useful for small systems, but limits the size of text objects.
 * <li> --enable-smallinputfile Disable support of files bigger than 64kiB. This reduces the tree footprint. Useful for small systems.
 * </ul>
 * \section build_end </building libroxml>
 */

/** \page public-api public API
 *
 * <div class="api">
 * <table>
 * <tr>
 * <td>
 * <h2>Manage XML source</h2>
 * <ul>
 * <li>\ref roxml_load_fd</li>
 * <li>\ref roxml_load_doc</li>
 * <li>\ref roxml_load_buf</li>
 * <li>\ref roxml_close</li>
 * </ul>
 * 
 * <h2>Access XML content</h2>
 * <ul>
 * <li>\ref roxml_get_type</li>
 * <li>\ref roxml_get_node_position</li>
 * <li>\ref roxml_get_name</li>
 * <li>\ref roxml_get_content</li>
 * </ul>
 * </td>
 * <td>
 * <h2>Navigate into XML tree</h2>
 * <ul>
 * <li>\ref roxml_get_root</li>
 * <li>\ref roxml_get_parent</li>
 * <li>\ref roxml_get_prev_sibling</li>
 * <li>\ref roxml_get_next_sibling</li>
 * <li>\ref roxml_get_ns</li>
 * <li>\ref roxml_get_chld</li>
 * <li>\ref roxml_get_chld_nb</li>
 * <li>\ref roxml_get_attr</li>
 * <li>\ref roxml_get_attr_nb</li>
 * <li>\ref roxml_get_txt</li>
 * <li>\ref roxml_get_txt_nb</li>
 * <li>\ref roxml_get_pi</li>
 * <li>\ref roxml_get_pi_nb</li>
 * <li>\ref roxml_get_cmt</li>
 * <li>\ref roxml_get_cmt_nb</li>
 * <li>\ref roxml_get_nodes</li>
 * <li>\ref roxml_get_nodes_nb</li>
 * <li>\ref roxml_xpath</li>
 * </ul>
 * </td>
 * <td>
 * <h2>Modify XML tree</h2>
 * <ul>
 * <li>\ref roxml_set_ns</li>
 * <li>\ref roxml_add_node</li>
 * <li>\ref roxml_del_node</li>
 * <li>\ref roxml_commit_changes</li>
 * <li>\ref roxml_commit_buffer</li>
 * <li>\ref roxml_commit_file</li>
 * </ul>
 *
 * <h2>Libroxml specifics</h2>
 * <ul>
 * <li>\ref roxml_release</li>
 * </ul>
 * </td>
 * </tr>
 * </table>
 * </div>
 */

/** \page priv-api private API
 * This is the documentation for private API. Users of libroxml should not need to go there. However, if you need to hack into library, understand how things are done, or correct a bug, feel free to dive in...
 * <div class="api">
 * <table>
 * <tr>
 * <td>
 * <h2>low level node handling</h2>
 * <ul>
 * <li>\ref roxml_free_node</li>
 * <li>\ref roxml_create_node</li>
 * <li>\ref roxml_del_tree</li>
 * <li>\ref roxml_close_node</li>
 * <li>\ref roxml_load</li>
 * <li>\ref roxml_parent_node</li>
 * <li>\ref roxml_process_begin_node</li>
 * </ul>
 *
 * <h2>misc functions</h2>
 * <ul>
 * <li>\ref roxml_malloc</li>
 * <li>\ref roxml_read</li>
 * <li>\ref roxml_is_separator</li>
 * </ul>
 * </td>
 * <td>
 * <h2>low level xpath functions</h2>
 * <ul>
 * <li>\ref roxml_set_axes</li>
 * <li>\ref roxml_parse_xpath</li>
 * <li>\ref roxml_free_xcond</li>
 * <li>\ref roxml_free_xpath</li>
 * <li>\ref roxml_double_cmp</li>
 * <li>\ref roxml_double_oper</li>
 * <li>\ref roxml_validate_predicat</li>
 * <li>\ref roxml_validate_axes</li>
 * <li>\ref roxml_check_node</li>
 * <li>\ref roxml_compute_and</li>
 * <li>\ref roxml_compute_or</li>
 * <li>\ref roxml_exec_xpath</li>
 * </ul>
 * </td>
 * <td>
 * <h2>node set handling</h2>
 * <ul>
 * <li>\ref roxml_request_id</li>
 * <li>\ref roxml_release_id</li>
 * <li>\ref roxml_add_to_pool</li>
 * <li>\ref roxml_del_from_pool</li>
 * <li>\ref roxml_in_pool</li>
 * </ul>
 *
 * <h2>modifications handling</h2>
 * <ul>
 * <li>\ref roxml_print_space</li>
 * <li>\ref roxml_write_string</li>
 * <li>\ref roxml_write_node</li>
 * <li>\ref roxml_del_arg_node</li>
 * <li>\ref roxml_del_txt_node</li>
 * <li>\ref roxml_del_std_node</li>
 * <li>\ref roxml_set_type</li>
 * <li>\ref roxml_get_node_internal_position</li>
 * </ul>
 * </td>
 * </tr>
 * </table>
 * </div>
 */

/** \page xpath xpath handling
 * <div class="xpath">
 * \section rel-xpath Relative xpath
 * <table border="0">
 * <tr><td> starting point from any given node</td><td><i>n1/n2/n3</i></td></tr>
 * </table border="0">
 *
 * \section abs-xpath Absolute xpath
 * <table border="0">
 * <tr><td> starting point form root even if current node is different</td><td><i>/n0/n1/n2/n3</i></td></tr>
 * </table border="0">
 *
 * \section spec-nodes Special Nodes
 * <table border="0">
 * <tr><td>access all nodes</td><td><i>*</i></td></tr>
 * <tr><td>access only element nodes</td><td><i>node()</i></td></tr>
 * <tr><td>access only text nodes</td><td>text()</td></tr>
 * <tr><td>access only comment nodes</td><td><i>comment()</i></td></tr>
 * <tr><td>access only processing-instruction nodes</td><td><i>processing-instruction()</i></td></tr>
 * </table border="0">
 *
 * \section cond Conditions
 * <table border="0">
 * <tr><td>Attribute string value</td><td><i>/n0[@a=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a!=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a<value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a>value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a<=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a>=value]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[first()]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[first()+2]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[last()]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[last()-3]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() = 0]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() != 0]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() > 1]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() < 2]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() >= 1]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() <= 2]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() <= 2]/n1</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[2]</i></td></tr>
 * <tr><td>Node with namespace</td><td><i>//*[namespace-uri() = 'http://test.com']</i></td></tr>
 * <tr><td>Other xpath</td><td><i>/n0[n1/\@attr]</i></td></tr>
 * </table border="0">
 *
 * \section short-xpath Shorten xpath
 * <table border="0">
 * <tr><td>Children</td><td><i>/n0/n1/n2</i></td></tr>
 * <tr><td>Attributes</td><td><i>/n0/n1/\@a</i></td></tr>
 * <tr><td>Descendent or self::<i>node()</td><td>/n0//n3</i></td></tr>
 * <tr><td>Parent</td><td><i>/n0/n1/../n1/n2</i></td></tr>
 * <tr><td>Self</td><td><i>/n0/n1/./n2</i></td></tr>
 * <tr><td>Node in namespaces</td><td><i>/test::n0</i></td></tr>
 * </table border="0">
 *
 * \section full-xpath Full xpath
 * <table border="0">
 * <tr><td>Nodes</td><td><i>/n0/n1/child::a</i></td></tr>
 * <tr><td>Attributes</td><td><i>/n0/n1/attribute::a</i></td></tr>
 * <tr><td>Descendent or self</td><td><i>/n0/descendant-or-self::n5</i></td></tr>
 * <tr><td>Parent</td><td><i>/child::n0/child::n1/parent::/n1</i></td></tr>
 * <tr><td>Self</td><td><i>/child::n0/child::n1/self::</i></td></tr>
 * <tr><td>Preceding</td><td><i>preceding::n1</i></td></tr>
 * <tr><td>Following</td><td><i>following::n1</i></td></tr>
 * <tr><td>Ancestor</td><td><i>ancestor::n2</i></td></tr>
 * <tr><td>Ancestor-or-self</td><td><i>ancestor-or-self::n2</i></td></tr>
 * <tr><td>Namespace</td><td><i>namespace::test</i></td></tr>
 * </table border="0">
 * </div>
 */

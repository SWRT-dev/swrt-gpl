#!/bin/sh
cd ../www
for i in $(find *.dict)
do
	sed -i '/Softcenter/d' $i
	sed -i "/Softcenter_tool=/d" $i
	sed -i "/WLANConfig11b_TxPower_custom=/d" $i
	sed -i '/Tencent Acceleration/d' $i
	sed -i '/TencentAcceleration/d' $i
	sed -i '/Entware_home/d' $i
	sed -i '/Entware_installed/d' $i
	sed -i '/Entware_list/d' $i
	sed -i '/Entware_mount/d' $i
	sed -i '/Entware_autorefresh/d' $i
	sed -i '/Entware_app_start/d' $i
	sed -i '/Entware_app_stop/d' $i
	sed -i '/Entware_app_none/d' $i
	sed -i '/Entware_usblist/d' $i
	sed -i '/Entware_usbport/d' $i
	sed -i '/Entware_usbsize/d' $i
	sed -i '/Entware_usbused/d' $i
	sed -i '/Entware_usbpermission/d' $i
	sed -i '/Entware_usbtype/d' $i
	sed -i '/Entware_usbmountp/d' $i
	sed -i '/Entware_usbpath/d' $i
	sed -i '/Entware_env/d' $i
	sed -i '/Entware_type1/d' $i
	sed -i '/Entware_type2/d' $i
	sed -i '/Entware_desc1/d' $i
	sed -i '/Entware_desc2/d' $i
	sed -i '/Entware_desc3/d' $i
	sed -i '/Entware_desc4/d' $i
	sed -i '/Entware_desc5/d' $i
	sed -i '/Entware_desc6/d' $i
	sed -i '/Entware_desc7/d' $i
	sed -i '/Entware_desc8/d' $i
	sed -i '/Entware_updateentware/d' $i
	sed -i '/Entware_service/d' $i
	sed -i '/Entware_operation/d' $i
	sed -i '/Entware_del/d' $i
	sed -i '/Entware_update\>/d' $i
	sed -i '/Entware_updatable/d' $i
	sed -i '/Entware_notfound/d' $i
	sed -i '/Entware_installedop/d' $i
	sed -i '/Entware_installedall/d' $i
	sed -i '/Entware_appname/d' $i
	sed -i '/Entware_appver/d' $i
	sed -i '/Entware_newapp/d' $i
	sed -i '/SmartDNS_desc/d' $i
	sed -i '/SmartDNS_dis_ipv6/d' $i
	sed -i '/SmartDNS_prefetch/d' $i
	sed -i '/SmartDNS_dualstackip/d' $i
	sed -i '/SmartDNS_serverdesc/d' $i
	if [ "$i" = "CN.dict" ];then
		echo "Softcenter=软件中心" >> $i
		echo "Softcenter_tool=工具箱" >> $i
		echo "TencentAcceleration=腾讯网游加速器" >> $i
		echo "WLANConfig11b_TxPower_custom=自定义功率" >> $i
		echo "Entware_home=Entware首页" >> $i
		echo "Entware_installed=已安装列表" >> $i
		echo "Entware_list=全部列表" >> $i
		echo "Entware_autorefresh=请等待系统完成目前操作，完成后页面会自动刷新..." >> $i
		echo "Entware_app_start=启动" >> $i
		echo "Entware_app_stop=停止" >> $i
		echo "Entware_app_none=目前无可自启服务" >> $i
		echo "Entware_usblist=磁盘分区列表" >> $i
		echo "Entware_usbport=端口" >> $i
		echo "Entware_usbsize=大小" >> $i
		echo "Entware_usbused=已用" >> $i
		echo "Entware_usbpermission=权限" >> $i
		echo "Entware_usbtype=格式" >> $i
		echo "Entware_usbmountp=挂载点" >> $i
		echo "Entware_usbpath=路径" >> $i
		echo "Entware_env=EntWare环境设置" >> $i
		echo "Entware_type1=使用系统环境设置(generic)" >> $i
		echo "Entware_type2=完全独立安装(alternative)" >> $i
		echo "Entware_desc1=Entware官方网址：entware.net，本系统将在线安装至USB设备的opt文件夹中." >> $i
		echo "Entware_desc2=目前Entware有两种版本：一、使用系统环境、busybox、用户密码等；二、完全独立安装（初始密码123456）." >> $i
		echo "Entware_desc3=为保证Entware正常，USB设备务必使用EXT3/4格式." >> $i
		echo "Entware_desc4=Entware无法和ASUS的aicloud/downloadmaster等同时启用." >> $i
		echo "Entware_desc5=目前未完成自动检测有无新版本." >> $i
		echo "Entware_desc6=如果删除支持库导致某程序不能正常运行, 请重新安装该程序." >> $i
		echo "Entware_desc7=如果想升级所有程序, 请到开始页进行更新操作." >> $i
		echo "Entware_desc8=下方有全部软件列表, 若要安装相应软件, 复制相应软件名到文本框, 点击安装即可." >> $i
		echo "Entware_updateentware=EntWare环境已安装，进行更新操作时将更新所有组件及程序." >> $i
		echo "Entware_service=自启动服务" >> $i
		echo "Entware_operation=操作" >> $i
		echo "Entware_del=确定要删除 " >> $i
		echo "Entware_update=确定要更新 " >> $i
		echo "Entware_updatable=可更新" >> $i
		echo "Entware_notfound=目前未安装程序" >> $i
		echo "Entware_installedop=已安装程序操作" >> $i
		echo "Entware_installedall=已安装程序（包括环境与支持库）" >> $i
		echo "Entware_appname=程序名称" >> $i
		echo "Entware_appver=版本" >> $i
		echo "Entware_mount=选择要挂载的分区" >> $i
		echo "Entware_newapp=安装新软件" >> $i
		echo "SmartDNS_desc=SmartDNS是一个本地高性能DNS服务器，支持避免域名污染，支持返回最快IP，支持广告过滤。" >> $i
		echo "SmartDNS_dis_ipv6=禁用ipv6解析" >> $i
		echo "SmartDNS_prefetch=域名预加载" >> $i
		echo "SmartDNS_dualstackip=双栈IP优选" >> $i
		echo "SmartDNS_serverdesc=上游服务器列表" >> $i
	elif [ "$i" = "TW.dict" ];then
		echo "Softcenter=軟體中心" >> $i
		echo "Softcenter_tool=工具箱" >> $i
		echo "TencentAcceleration=騰訊網遊加速器" >> $i
		echo "WLANConfig11b_TxPower_custom=自訂功率" >> $i
		echo "Entware_home=Entware首頁" >> $i
		echo "Entware_installed=已安裝列表" >> $i
		echo "Entware_list=全部清單" >> $i
		echo "Entware_autorefresh=請等待系統完成目前操作，完成後頁面會自動刷新..." >> $i
		echo "Entware_app_start=啟動" >> $i
		echo "Entware_app_stop=停止" >> $i
		echo "Entware_app_none=目前無可自啟服務" >> $i
		echo "Entware_usblist=磁碟分割列表" >> $i
		echo "Entware_usbport=埠" >> $i
		echo "Entware_usbsize=大小" >> $i
		echo "Entware_usbused=已用" >> $i
		echo "Entware_usbpermission=許可權" >> $i
		echo "Entware_usbtype=格式" >> $i
		echo "Entware_usbmountp=掛載點" >> $i
		echo "Entware_usbpath=路徑" >> $i
		echo "Entware_env=EntWare環境設置" >> $i
		echo "Entware_type1=使用系統環境設置(generic)" >> $i
		echo "Entware_type2=完全獨立安裝(alternative)" >> $i
		echo "Entware_desc1=Entware官方網址：entware.net，本系統將線上安裝至USB設備的opt資料夾中." >> $i
		echo "Entware_desc2=目前Entware有兩種版本：一、使用系統環境、busybox、使用者密碼等；二、完全獨立安裝（初始密碼123456）." >> $i
		echo "Entware_desc3=為保證Entware正常，USB設備務必使用EXT3/4格式." >> $i
		echo "Entware_desc4=Entware無法和ASUS的aicloud/downloadmaster等同時啟用." >> $i
		echo "Entware_desc5=目前未完成自動檢測有無新版本." >> $i
		echo "Entware_desc6=如果刪除支援函式庫導致某程式不能正常運行, 請重新安裝該程式." >> $i
		echo "Entware_desc7=如果想升級所有程式, 請到開始頁進行更新操作." >> $i
		echo "Entware_desc8=下方有全部軟體清單, 若要安裝相應軟體, 複製相應軟體名到文字方塊, 點擊安裝即可." >> $i
		echo "Entware_updateentware=EntWare環境已安裝，進行更新操作時將更新所有元件及程式." >> $i
		echo "Entware_service=自啟動服務" >> $i
		echo "Entware_operation=操作" >> $i
		echo "Entware_del=確定要刪除 " >> $i
		echo "Entware_update=確定要更新 " >> $i
		echo "Entware_updatable=可更新" >> $i
		echo "Entware_notfound=目前未安裝程式" >> $i
		echo "Entware_installedop=已安裝程式操作" >> $i
		echo "Entware_installedall=已安裝程式（包括環境與支援函式庫）" >> $i
		echo "Entware_appname=程式名稱" >> $i
		echo "Entware_appver=版本" >> $i
		echo "Entware_mount=選擇要掛載的分區" >> $i
		echo "Entware_newapp=安裝新軟體" >> $i
		echo "SmartDNS_desc=SmartDNS是一个本地高性能DNS服务器，支持避免域名污染，支持返回最快IP，支持广告过滤。" >> $i
		echo "SmartDNS_dis_ipv6=禁用ipv6解析" >> $i
		echo "SmartDNS_prefetch=域名预加载" >> $i
		echo "SmartDNS_dualstackip=双栈IP优选" >> $i
		echo "SmartDNS_serverdesc=上游服务器列表" >> $i
	elif [ "$i" = "RU.dict" ];then
		echo "Softcenter=Softcenter" >> $i
		echo "Softcenter_tool=Инструменты" >> $i
		echo "TencentAcceleration=Tencent Acceleration" >> $i
		echo "WLANConfig11b_TxPower_custom=Пользовательский txpower" >> $i
		echo "Entware_home=Entware home" >> $i
		echo "Entware_installed=Список установленных" >> $i
		echo "Entware_list=Список всех" >> $i
		echo "Entware_autorefresh=Пожалуйста, подождите. Страница автоматически обновится после завершения текущей операции..." >> $i
		echo "Entware_app_start=Start" >> $i
		echo "Entware_app_stop=Stop" >> $i
		echo "Entware_app_none=Не найдено" >> $i
		echo "Entware_usblist=Список разделов диска " >> $i
		echo "Entware_usbport=Порт" >> $i
		echo "Entware_usbsize=Объём" >> $i
		echo "Entware_usbused=Использовано" >> $i
		echo "Entware_usbpermission=Разрешение" >> $i
		echo "Entware_usbtype=Тип" >> $i
		echo "Entware_usbmountp=Точка монтирования" >> $i
		echo "Entware_usbpath=Путь" >> $i
		echo "Entware_env=Параметры среды EntWare" >> $i
		echo "Entware_type1=Общий" >> $i
		echo "Entware_type2=Альтернативный" >> $i
		echo "Entware_desc1=Официальный сайт Entware — entware.net. Система будет установлена онлайн в папку opt на USB-устройстве." >> $i
		echo "Entware_desc2=В настоящее время существует две версии entware: 1、 Использует системную среду, busybox, пароль пользователя и т.д.; 2、 Полностью самостоятельная установка (начальный пароль 123456)" >> $i
		echo "Entware_desc3=Чтобы убедиться, что Entware работает нормально, USB-устройства должны использовать формат EXT3/4." >> $i
		echo "Entware_desc4=Entware нельзя включить одновременно с ASUS aicloud/downloadmaster." >> $i
		echo "Entware_desc5=В настоящее время автоматическое определение наличия новой версии не работает." >> $i
		echo "Entware_desc6=Если вспомогательная библиотека удалена, программа не cможет функционировать должным образом. Пожалуйста, переустановите программу." >> $i
		echo "Entware_desc7=Если вы хотите обновить все программы, перейдите на стартовую страницу для обновления." >> $i
		echo "Entware_desc8=Ниже приведены все списки программного обеспечения. Если вы хотите установить соответствующее программное обеспечение, скопируйте название соответствующего программного обеспечения в текстовое поле, затем нажмите кнопку «Установить».." >> $i
		echo "Entware_updateentware=Среда Entware была установлена. Все компоненты и программы будут обновлены в ходе обновления." >> $i
		echo "Entware_service=Служба запуска" >> $i
		echo "Entware_operation=Operation" >> $i
		echo "Entware_del=Подтвердить удаление " >> $i
		echo "Entware_update=Подтвердить обновление " >> $i
		echo "Entware_updatable=Обновляемый" >> $i
		echo "Entware_notfound=В настоящее время программа не установлена." >> $i
		echo "Entware_installedop=Installed program operation" >> $i
		echo "Entware_installedall=Установленные программы (включая окружение и библиотеки)" >> $i
		echo "Entware_appname=Название программы" >> $i
		echo "Entware_appver=Версия программы" >> $i
		echo "Entware_mount=Выберите раздел для монтирования" >> $i
		echo "Entware_newapp=Установить новую программу" >> $i
		echo "SmartDNS_desc=SmartDNS — это локальный DNS-сервер. SmartDNS принимает запросы DNS-запросов от локальных клиентов, получает результаты DNS-запросов от нескольких вышестоящих DNS-серверов и возвращает клиентам самые быстрые результаты доступа." >> $i
		echo "SmartDNS_dis_ipv6=Отключить результаты запроса IPv6" >> $i
		echo "SmartDNS_prefetch=Предзагрузка домена" >> $i
		echo "SmartDNS_dualstackip=Выбор двойного стека ip" >> $i
		echo "SmartDNS_serverdesc=Список вышестоящих DNS-серверов" >> $i
	else
		echo "Softcenter=Softcenter" >> $i
		echo "Softcenter_tool=Tools" >> $i
		echo "TencentAcceleration=Tencent Acceleration" >> $i
		echo "WLANConfig11b_TxPower_custom=Custom txpower" >> $i
		echo "Entware_home=Entware home" >> $i
		echo "Entware_installed=List of installed" >> $i
		echo "Entware_list=List of all" >> $i
		echo "Entware_autorefresh=Please wait. The page will refresh automatically after the current operation is completed..." >> $i
		echo "Entware_app_start=Start" >> $i
		echo "Entware_app_stop=Stop" >> $i
		echo "Entware_app_none=Not found" >> $i
		echo "Entware_usblist=Disk partition list " >> $i
		echo "Entware_usbport=Port" >> $i
		echo "Entware_usbsize=Capacity" >> $i
		echo "Entware_usbused=Used" >> $i
		echo "Entware_usbpermission=Permission" >> $i
		echo "Entware_usbtype=Type" >> $i
		echo "Entware_usbmountp=Mount point" >> $i
		echo "Entware_usbpath=Path" >> $i
		echo "Entware_env=EntWare environment settings" >> $i
		echo "Entware_type1=Generic" >> $i
		echo "Entware_type2=Alternative" >> $i
		echo "Entware_desc1=The official website of entware is entware.net. The system will be installed online in the opt folder of USB device." >> $i
		echo "Entware_desc2=At present, there are two versions of entware: first, use system environment, busybox, user password, etc; 2、 Completely independent installation (initial password 123456)" >> $i
		echo "Entware_desc3=To ensure Entware is normal, USB devices must use EXT3/4 format." >> $i
		echo "Entware_desc4=Entware cannot be enabled at the same time as aicloud/downloadmaster of ASUS." >> $i
		echo "Entware_desc5=At present, the automatic detection of whether there is a new version has not been completed" >> $i
		echo "Entware_desc6=If the support library is deleted, a program can not function properly. Please reinstall the program." >> $i
		echo "Entware_desc7=If you want to upgrade all programs, please go to the start page to update." >> $i
		echo "Entware_desc8=There are all software lists below. If you want to install the corresponding software, copy the corresponding software name to the text box, then click install button." >> $i
		echo "Entware_updateentware=The entware environment has been installed. All components and programs will be updated during the update operation." >> $i
		echo "Entware_service=Startup service" >> $i
		echo "Entware_operation=Operation" >> $i
		echo "Entware_del=Confirm to delete " >> $i
		echo "Entware_update=Confirm to update " >> $i
		echo "Entware_updatable=Updatable" >> $i
		echo "Entware_notfound=No program is currently installed." >> $i
		echo "Entware_installedop=Installed program operation" >> $i
		echo "Entware_installedall=Installed programs (including environment and libraries)" >> $i
		echo "Entware_appname=Program name" >> $i
		echo "Entware_appver=Program version" >> $i
		echo "Entware_mount=Select a partition to mount" >> $i
		echo "Entware_newapp=Install new program" >> $i
		echo "SmartDNS_desc=SmartDNS is a local DNS server. SmartDNS accepts DNS query requests from local clients, obtains DNS query results from multiple upstream DNS servers, and returns the fastest access results to clients." >> $i
		echo "SmartDNS_dis_ipv6=Disable ipv6 query results" >> $i
		echo "SmartDNS_prefetch=Domain prefetch" >> $i
		echo "SmartDNS_dualstackip=Dualstack ip selection" >> $i
		echo "SmartDNS_serverdesc=Upstream DNS server list" >> $i
	fi
done


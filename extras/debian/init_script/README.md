Init Script For debian
==========
Script to make OpenJabNab run at Boot time.


Installation
----------------
1. edit openjabnab and change the OJN_PATH variable to the absolute path of your OpenJabNab/server/bin/ directory

2. copy the openjabnab file to /etc/init.d
$: cp openjabnab /etc/init.d/

3. Make it executable
$: chmod 755 /etc/init.d/openjabnab

4. Register it:
$: update-rc.d openjabnab defaults

(5). To remove it:
$: update-rc.d -f openjabnab remove

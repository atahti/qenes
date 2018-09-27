#
# Regular cron jobs for the qenes package
#
0 4	* * *	root	[ -x /usr/bin/qenes_maintenance ] && /usr/bin/qenes_maintenance

#
# Regular cron jobs for the fdenavi package
#
0 4	* * *	root	[ -x /usr/bin/fdenavi_maintenance ] && /usr/bin/fdenavi_maintenance

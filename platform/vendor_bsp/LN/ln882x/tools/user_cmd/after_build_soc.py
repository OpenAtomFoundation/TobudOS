import os
import sys
import time

prj_name = sys.argv[1]
cur_dir = os.getcwd()
print("Project: %s" % (prj_name))
print("Current dir: %s" % (cur_dir))
axf_path = os.getcwd() + '\\Objects\\' + prj_name + '.axf'
print(axf_path)

print("Generate asm file")
os.system("fromelf -c -v -a --output=" + prj_name + ".asm " + axf_path)
print("Asm done")

print("Generate bin file")
os.system("fromelf --bin --output=" + prj_name + ".bin " + axf_path)
now_time = time.localtime()
time_string = "{yyyy}-{MM:02d}-{DD:02d} {hh:02d}:{mm:02d}:{ss:02d}".format(yyyy=now_time.tm_year,
			   MM=now_time.tm_mon, DD=now_time.tm_mday,
			   hh=now_time.tm_hour, mm=now_time.tm_min, ss=now_time.tm_sec)
print("----------" * 6)
print("\t\t\tbin done @ {t}".format(t=time_string))
print("----------" * 6)

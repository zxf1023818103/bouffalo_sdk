import sys,os,shutil
import hashlib, sys, binascii, struct
import time
import subprocess
import re
import zipfile
import argparse

git_cmd=r'git describe --tags '#--dirty
tag_standard="boot2_v1.0.0"

release_list = [
[r'make clean;make CHIP=bl602            BOARD=bl602dk CONFIG_DEBUG=n',"bl602","release"],
[r'make clean;make CHIP=bl602            BOARD=bl602dk CONFIG_DEBUG=y',"bl602","debug"],
[r'make clean;make CHIP=bl702            BOARD=bl702dk CONFIG_DEBUG=n',"bl702","release"],
[r'make clean;make CHIP=bl702            BOARD=bl702dk CONFIG_DEBUG=y',"bl702","debug"],
[r'make clean;make CHIP=bl702l CPU_ID=m0 BOARD=bl702ldk CONFIG_DEBUG=n',"bl702l","release"],
[r'make clean;make CHIP=bl702l CPU_ID=m0 BOARD=bl702ldk CONFIG_DEBUG=y',"bl702l","debug"],
[r'make clean;make CHIP=bl808  CPU_ID=m0 BOARD=bl808dk CONFIG_DEBUG=n',"bl808","release"],
[r'make clean;make CHIP=bl808  CPU_ID=m0 BOARD=bl808dk CONFIG_DEBUG=y',"bl808","debug"],
[r'make clean;make CHIP=bl606p CPU_ID=m0 BOARD=bl606pdk CONFIG_DEBUG=n',"bl606p","release"],
[r'make clean;make CHIP=bl606p CPU_ID=m0 BOARD=bl606pdk CONFIG_DEBUG=y',"bl606p","debug"],
[r'make clean;make CHIP=bl616  CPU_ID=m0 BOARD=bl616dk CONFIG_ANTI_ROLLBACK=y CONFIG_DEBUG=n',"bl616","release"],
[r'make clean;make CHIP=bl616  CPU_ID=m0 BOARD=bl616dk CONFIG_ANTI_ROLLBACK=y CONFIG_DEBUG=y',"bl616","debug"],
]



def zipDir(dirpath,outFullName):
    """
    压缩指定文件夹
    :param dirpath: 目标文件夹路径
    :param outFullName: 压缩文件保存路径+xxxx.zip
    :return: 无
    """
    zip = zipfile.ZipFile(outFullName,"w",zipfile.ZIP_DEFLATED)
    for path,dirnames,filenames in os.walk(dirpath):
        # 去掉目标跟路径，只对目标文件夹下边的文件及文件夹进行压缩
        fpath = path.replace(dirpath,'')

        for filename in filenames:
            zip.write(os.path.join(path,filename),os.path.join(fpath,filename))
    zip.close()

def recreate_release_dir(dir):
    if os.path.exists(dir):
        shutil.rmtree(dir)#删除再建立
        os.makedirs(dir)
    else:
        os.makedirs(dir)

def boot2_release(cmd, ver):
    print("Executing command:", cmd)

    process = subprocess.Popen(
        f"{cmd[0]} CONFIG_BOOT2_VER={ver}",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=True,
        text=True
    )

    output, err = process.communicate()

    if process.returncode != 0:
        print("Build error:", err.strip())
        return False

    print(output.strip())

    target_dir = f'boot2_isp_{cmd[1]}_v{ver}'
    for root, dirs, files in os.walk("./build/build_out", topdown=False):
        for file in files:
            if file.endswith(".bin") and cmd[1] in file and "boot2_isp" in file:
                src = os.path.join(root, file)
                dest = os.path.join(target_dir, f'boot2_isp_{cmd[2]}.bin')
                shutil.copy(src, dest)
                print(f"Copied: {src} -> {dest}\n")
                return

def get_release_ver():

    print("get release ver")
    output,err = subprocess.Popen(git_cmd,stdout=subprocess.PIPE,shell=True).communicate()
    if err==None:
        release_name = bytes.decode(output)
        release_name = release_name.rstrip()


        if(len(release_name) != len(tag_standard)):
            print("ver len err, workspace may dirty? or has no tag?")
            print("release failed!")
            sys.exit()
        release_name = release_name[7:12]
        return release_name

    else:
        print("get ver error,please push correct tag")
        sys.exit()



if __name__ == '__main__' :

    ver = get_release_ver()
    print(ver)

    failed_list = []

    for x in release_list:
        recreate_release_dir('boot2_isp_' + x[1] + '_v' + ver )
    for x in release_list:
        if boot2_release(x,ver) == False:
            failed_list.append(x)

    if len(failed_list) == 0:
        print("release success!")
    else:
        print("release failed! failed list:")
        for x in failed_list:
            print('-',x[1],x[2])
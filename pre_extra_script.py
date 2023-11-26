Import("env")
from shutil import copyfile

# copy User_Setup.h configuration to downloaded library
projectDir=env.get('PROJECT_DIR')
print(projectDir)
copyfile(projectDir+"/include/User_Setup.h", projectDir+"/.pio/libdeps/esp32dev/TFT_eSPI/User_Setup.h")

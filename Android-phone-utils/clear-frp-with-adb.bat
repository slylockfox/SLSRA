REM see https://blog.sombex.com/2018/01/all-adb-and-fastboot-commands-to-remove-frp-lock-on-android-phones.html
adb shell content insert --uri content://settings/secure --bind name:s:user_setup_complete --bind value:s:1
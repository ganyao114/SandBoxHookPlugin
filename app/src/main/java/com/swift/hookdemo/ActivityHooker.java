package com.swift.hookdemo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.MethodParams;

@HookClass(Activity.class)
public class ActivityHooker {


    @HookMethod("onCreate")
    @MethodParams(Bundle.class)
    public static void onCreate(Activity thiz, Bundle bundle) {
        Log.e("ActivityHooker", "hooked onCreate success " + thiz);
        Toast.makeText(thiz, thiz.getClass().getName(), Toast.LENGTH_SHORT).show();
        onCreateBackup(thiz, bundle);
    }

    @HookMethodBackup("onCreate")
    @MethodParams(Bundle.class)
    public static void onCreateBackup(Activity thiz, Bundle bundle) {
        //invoke self to kill inline
        onCreateBackup(thiz, bundle);
    }

    @HookMethod("onPause")
    public static void onPause(Activity thiz) {
        Log.e("ActivityHooker", "hooked onPause success " + thiz);
        onPauseBackup(thiz);
    }

    @HookMethodBackup("onPause")
    public static void onPauseBackup(Activity thiz) {
        //invoke self to kill inline
        onPauseBackup(thiz);
    }

}

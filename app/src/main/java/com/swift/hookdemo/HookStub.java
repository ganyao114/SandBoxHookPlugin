package com.swift.hookdemo;

import android.os.Environment;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.SandHookConfig;
import com.swift.sandhook.wrapper.HookErrorException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


public class HookStub {

    public final static String targetPkg = "com.trendmicro.speedy";


    public static void onInjected() {
        String sdOldPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Hook/libsandhook32.so";
        String soNewPath = "/data/user/0/com.trendmicro.tmas.debug/sandboxdata/com.swift.hookdemo/data/files/Hook/libsandhook32.so";
        try {
            copyFile(new File(sdOldPath), new File(soNewPath));
        } catch (IOException e) {
            e.printStackTrace();
        }
        SandHookConfig.libSandHookPath = soNewPath;
        try {
            SandHook.addHookClass(ActivityHooker.class);
        } catch (HookErrorException e) {
            e.printStackTrace();
        }
    }


    private static void copyFile(File source, File dest)
            throws IOException {
        InputStream input = null;
        OutputStream output = null;
        try {
            input = new FileInputStream(source);
            output = new FileOutputStream(dest);
            byte[] buf = new byte[1024];
            int bytesRead;
            while ((bytesRead = input.read(buf)) > 0) {
                output.write(buf, 0, bytesRead);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            input.close();
            output.close();
        }
    }
}
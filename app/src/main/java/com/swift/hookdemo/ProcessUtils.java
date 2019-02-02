package com.swift.hookdemo;

import android.os.Build;
import android.text.TextUtils;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class ProcessUtils {

    public static int getPid(String processName) {
        String cmd;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            cmd = "ps -A";
        } else {
            cmd = "ps";
        }
        String[] resLines = exec(cmd);
        if (resLines == null || resLines.length <= 1)
            return -1;

        String[] columsTitle = resLines[0].split("\\s+");
        if (columsTitle == null || columsTitle.length <= 1)
            return -1;

        int pidIndex = -1;

        for (int i = 0;i < columsTitle.length;i++) {
            if (TextUtils.equals(columsTitle[i].toLowerCase(), "pid")) {
                pidIndex = i;
                break;
            }
        }

        if (pidIndex < 0)
            return -1;

        for (int i = 1;i < resLines.length;i++) {
            String[] colums = resLines[i].split("\\s+");
            if (colums == null || colums.length < pidIndex + 1)
                continue;
            for (int j = 0;j < colums.length;j++) {
                if (colums[j].contains(processName)) {
                    try {
                        return Integer.parseInt(colums[pidIndex]);
                    } catch (Exception e) {}
                }
            }
        }

        return -1;

    }

    public static boolean is64Bit(String processName) {
        return true;
    }

    public static String[] exec(String cmd) {
        Process process = null;
        BufferedReader is = null;
        String result = "";
        try {
            process = Runtime.getRuntime().exec(cmd);
            is = new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line;
            while ((line = is.readLine()) != null) {
                result = result + line + "\n";
            }
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        } finally {
            try {
                process.destroy();
            } catch (Exception e) {
            }
        }
        return result.split("\n");
    }


}

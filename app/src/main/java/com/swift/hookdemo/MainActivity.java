package com.swift.hookdemo;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static com.swift.hookdemo.HookStub.targetPkg;

public class MainActivity extends Activity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("injector");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.inject).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                doInject();
            }
        });
    }

    private void doInject() {
        String outDir = getFilesDir().getAbsolutePath() + "/Hook";
        String sdDir = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Hook";
        File sdFile = new File(sdDir);
        if (!sdFile.exists()) {
            sdFile.mkdirs();
        }
        try {
            writeAsset(outDir + "/libpayload32.so", "armeabi-v7a/libpayload.so");
            writeAsset(sdDir + "/libsandhook32.so", "armeabi-v7a/libsandhook");

            inject(targetPkg, outDir + "/libpayload32.so");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void writeAsset(String outfile, String assetfile) throws IOException {
        File file = new File(outfile);
        file.mkdirs();
        if (file.exists()) {
            file.delete();
        }
        InputStream inputStream = getAssets().open(assetfile);
        byte data[] = new byte[inputStream.available()];
        inputStream.read(data, 0, data.length);
        OutputStream outputStream = new FileOutputStream(file);
        outputStream.write(data);
        inputStream.close();
        outputStream.close();
        Log.d("inject", "writeAsset: " + assetfile);
    }


    public native void inject(String pkgname, String payload);

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
}

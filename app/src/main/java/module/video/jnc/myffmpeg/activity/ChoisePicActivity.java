package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.provider.MediaStore;
import android.support.v4.content.FileProvider;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.BuildConfig;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.ChoiseVideoAdapter;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.adapter.SelectedVideoAdapter;
import module.video.jnc.myffmpeg.bean.FileBean;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.DividerGridItemDecoration;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class ChoisePicActivity extends BaseActivity implements MyBaseAdapter.OnRecyleItemClick<FileBean> {
    private static final String root = "sdcard/DCIM/Camera";
    private RecyclerView recyclerView;
    private ChoiseVideoAdapter adapter;
    private SelectedVideoAdapter selectedVideoAdapter;
    private List<FileBean> listFile = new ArrayList<FileBean>();
    private String[] videoMIME = {"jpg", "png"};
    private RecyclerView rcChoiseView;
    private TitleBar titleBar;
    private ArrayList<String> listSelected = new ArrayList<>();
    private int choiseMaxNum;//选择图片的最大数
    private int choiseMin; //选择图片最小数
    private String actionNext;
    private static final int CROP_PHOTO = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choise_pic);
        Intent intent = getIntent();
        if (intent != null) {
            choiseMaxNum = intent.getIntExtra("choise_max_num_pic", 1);
            choiseMin = intent.getIntExtra("choise_min_pic", 1);
            actionNext = intent.getStringExtra("action_next");
            Log.e("xhc", " action next " + actionNext);
        }

        recyclerView = findViewById(R.id.recycler_view);
        rcChoiseView = findViewById(R.id.rl_choise);
        titleBar = findViewById(R.id.title);
        findFile(root);
        adapter = new ChoiseVideoAdapter(listFile, this);
        recyclerView.setLayoutManager(new GridLayoutManager(this, 3));
        recyclerView.addItemDecoration(new DividerGridItemDecoration(this));
        recyclerView.setAdapter(adapter);
        adapter.setOnRecyleItemClick(this);

        selectedVideoAdapter = new SelectedVideoAdapter(listSelected, this);
        rcChoiseView.setLayoutManager(new LinearLayoutManager(this, LinearLayoutManager.HORIZONTAL, false));
        rcChoiseView.setAdapter(selectedVideoAdapter);

        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                if (listSelected.size() <= 0) {
                    showToast("请选择图片！");
                    return;
                }
                if (listSelected.size() < choiseMin) {
                    showToast("至少选择 " + choiseMin + " 个图片！");
                    return;
                }
                cropPic(listSelected.get(0));
            }
        });
    }

    private void cropPic(String imagePath) {
        File file = new File(imagePath);
        Intent intent = new Intent("com.android.camera.action.CROP");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            Uri contentUri = FileProvider.getUriForFile(this, BuildConfig.APPLICATION_ID + ".fileProvider", file);
            intent.setDataAndType(contentUri, "image/*");
        } else {
            intent.setDataAndType(Uri.fromFile(file), "image/*");
        }
        intent.putExtra("crop", "true");
        intent.putExtra("aspectX", 2);
        intent.putExtra("aspectY", 1);
        intent.putExtra("outputX", 200);
        intent.putExtra("outputY", 100);
        intent.putExtra("return-data", true);
        intent.putExtra("scale", true);
        startActivityForResult(intent, CROP_PHOTO);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case CROP_PHOTO: //裁剪照片后
                if (data != null && data.getExtras() != null) {
                    Bitmap bitmap = data.getExtras().getParcelable("data");
                    String path = FileUtils.saveBitmapToWaterMark("water_mark" + System.currentTimeMillis() + ".jpg", bitmap);
                    bitmap.recycle();

                    Intent intent = new Intent();//ChoisePicActivity.this.getIntent(); //这里使用这个为什么会直接跳转自己的界面。
                    intent.setAction(actionNext);

                    intent.putExtra("choise_max_video" , getIntent().getIntExtra("choise_max_video" , 1));
                    intent.putExtra("choise_min_video" , getIntent().getIntExtra("choise_min_video" , 1));
                    intent.putExtra("action" , getIntent().getStringExtra("action"));
                    intent.putExtra("pic", path);
                    startActivity(intent);
                    finish();
                }
        }
    }

    private void findFile(String root) {
        File file = new File(root);

        File[] files = file.listFiles();
        if (files == null) return;
        for (File f : files) {
            if (isPic(f.getAbsolutePath())) {
                FileBean fb = new FileBean();
                fb.setPath(f.getAbsolutePath());
                listFile.add(fb);
            }
        }
    }

    private boolean isPic(String path) {
        if (TextUtils.isEmpty(path)) {
            return false;
        }
        for (int i = 0; i < videoMIME.length; ++i) {
            if (path.endsWith(videoMIME[i])) {
                return true;
            }
        }
        return false;
    }

    @Override
    public void onItemClick(View v, FileBean s, int position) {
        if (listSelected.size() >= choiseMaxNum && !s.isChoise()) {
            showToast("只能选取 " + choiseMaxNum + " 张");
            return;
        }
        if (s.isChoise()) {
            s.setChoise(false);
            listSelected.remove(s.getPath());
        } else {
            s.setChoise(true);
            listSelected.add(s.getPath());
        }
        adapter.refreshAllData(listFile);
        selectedVideoAdapter.refreshAllData(listSelected);
    }

}

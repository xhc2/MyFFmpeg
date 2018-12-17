package module.video.jnc.myffmpeg.activity;

import android.support.v4.app.Fragment;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.HomeViewPagerAdapter;
import module.video.jnc.myffmpeg.fragment.EditFragment;
import module.video.jnc.myffmpeg.fragment.OtherFragment;
import module.video.jnc.myffmpeg.fragment.PFragment;

public class MainActivity extends BaseActivity implements View.OnClickListener {

    private ViewPager viewPager;
    private HomeViewPagerAdapter adapter;
    private EditFragment editFragment;
    private PFragment playFragment;
    private OtherFragment otherFragment;
    private List<Fragment> listFragment = new ArrayList<>();
    private Button btVideoEdit;
    private Button btOther;
    private Button btPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        findViewById();
        init();

    }

    private void findViewById() {
        viewPager = findViewById(R.id.view_pager);
        btVideoEdit = findViewById(R.id.bt_edit);
        btOther = findViewById(R.id.bt_other);
        btPlayer = findViewById(R.id.bt_play);

    }

    private void init() {
        editFragment = new EditFragment();
        playFragment = new PFragment();
        otherFragment = new OtherFragment();
        listFragment.add(editFragment);
        listFragment.add(otherFragment);
        listFragment.add(playFragment);
        adapter = new HomeViewPagerAdapter(getSupportFragmentManager(), listFragment);
        viewPager.setAdapter(adapter);

        btVideoEdit.setOnClickListener(this);
        btOther.setOnClickListener(this);
        btPlayer.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.bt_edit:
                viewPager.setCurrentItem(0);
                break;
            case R.id.bt_other:
                viewPager.setCurrentItem(1);
                break;
            case R.id.bt_play:
                viewPager.setCurrentItem(2);
                break;
        }
    }
}

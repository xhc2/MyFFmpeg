package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import module.video.jnc.myffmpeg.tool.DensityUtils;

public class ClipBar extends View {

    private int width;
    private int height;
    private RectF bar;
    private RectF rectStart;
    private RectF rectEnd;
    private Paint paintBar;
    private Paint paintTouchBar;
    private float startPX = -1;
    private float endPX = -1;
    private float tempX;
    private int touchBarWidth;
    private int margin;
    private int roundCorner;
    private boolean moveStart;
    private boolean moveEnd;
    private static final int MAXPROGRESS = 100;

    private TouchCallBack callBack;

    public ClipBar(Context context) {
        this(context, null);
    }

    public ClipBar(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public ClipBar(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    public void setTouchCallBack(TouchCallBack touchCallBack){
        this.callBack = touchCallBack;
    }

    public interface TouchCallBack{
        void moveStart(float screenStartX , int startProgress);
        void moveEnd(float screenEndX , int endProgress);
        void moveFinish(  int startProgress , int endProgress);
    }

    private void init(Context context) {
        paintBar = new Paint();
        paintBar.setAntiAlias(true);
        paintBar.setColor(Color.parseColor("#4444BB"));

        paintTouchBar = new Paint();
        paintTouchBar.setAntiAlias(true);
        paintTouchBar.setColor(Color.parseColor("#55AA77"));


        margin = DensityUtils.dip2px(context, 5);

        touchBarWidth = DensityUtils.dip2px(context, 15);
        roundCorner = DensityUtils.dip2px(context, 10);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        width = getMeasuredWidth();
        height = getMeasuredHeight();
        bar = new RectF(0, margin, width, height - margin);
        if(startPX == -1 ){
            startPX = 0;
        }
        if(endPX == -1){
            endPX = width - touchBarWidth;
        }

        Log.e("xhc" , " onMeasure ");
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {


        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                tempX = event.getX();
                if (isInStartBar(tempX)) {
                    moveStart = true;
                    moveEnd = false;
                } else if (isInEndBar(tempX)) {
                    moveStart = false;
                    moveEnd = true;
                }
                break;
            case MotionEvent.ACTION_MOVE:
                tempX = event.getX();
                if (moveStart) {
                    if ((tempX + touchBarWidth) < endPX && tempX >= 0) {
                        startPX = tempX;
                        Log.e("xhc" ," touch event "+startPX);
                        invalidate();
                        if(callBack != null){
                            callBack.moveStart(event.getRawX() , (int)((startPX / width) * MAXPROGRESS));
                        }
                        Log.e("xhc" ," touch event 2"+startPX);

                    }
                } else if (moveEnd) {
                    if ((tempX - touchBarWidth) > startPX && (tempX + touchBarWidth) <= width) {
                        endPX = tempX;
                        invalidate();
                        if(callBack != null){
                            callBack.moveEnd(event.getRawX() , (int)(((endPX + touchBarWidth) / width) * MAXPROGRESS));
                        }
                    }
                }
                break;
            case MotionEvent.ACTION_UP:
                tempX = event.getX();
                if (moveStart) {

                } else if (moveEnd) {

                }
                if(callBack != null){
                    callBack.moveFinish((int)((startPX / width) * MAXPROGRESS) ,  (int)(((endPX + touchBarWidth) / width) * MAXPROGRESS));
                }

                moveStart = false;
                moveEnd = false;
                break;
        }

        return true;
    }

    private boolean isInStartBar(float x) {

        return (x >= startPX && x <= (startPX + touchBarWidth));

    }

    private boolean isInEndBar(float x) {
        return (x > endPX && x <= (endPX + touchBarWidth));
    }

    public int getMaxProgress(){
        return MAXPROGRESS;
    }

    @Override
    public void draw(Canvas canvas) {
        super.draw(canvas);
        drawBar(canvas);
        drawStartBar(canvas);
        drawEndBar(canvas);

    }

    private void drawStartBar(Canvas canvas){
        rectStart = new RectF(startPX, 0, touchBarWidth + startPX, height);
        canvas.drawRoundRect(rectStart, roundCorner, roundCorner, paintTouchBar);
        Log.e("xhc" , " draw start "+startPX);
    }

    private void drawEndBar(Canvas canvas){
        rectEnd = new RectF(endPX, 0, touchBarWidth + endPX, height);
        canvas.drawRoundRect(rectEnd, roundCorner, roundCorner, paintTouchBar);
    }

    private void drawBar(Canvas canvas) {
        canvas.drawRoundRect(bar, roundCorner, roundCorner, paintBar);
    }
}

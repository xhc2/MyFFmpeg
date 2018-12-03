package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import module.video.jnc.myffmpeg.tool.DensityUtils;

/**
 * 用来绘制裁剪区域
 */
public class CropGraph extends View implements View.OnTouchListener {

    private float xRect;
    private float yRect;
    private int widthRect;
    private int heightRect;
    private RectF rect;
    private Paint paintRect;
    private Paint paintPoint;
    private int circleRadius;
    private int minSize;

    private boolean ltFlag = false;
    private boolean lbFlag = false;
    private boolean rtFlag = false;
    private boolean rbFlag = false;

    private float tempX;
    private float tempY;

    public CropGraph(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        minSize = DensityUtils.dip2px(context, 50);
        xRect = 50;
        yRect = 50;
        widthRect = minSize;
        heightRect = minSize;
        rect = new RectF(xRect, yRect, widthRect + xRect, heightRect + yRect);
        paintRect = new Paint();
        paintRect.setColor(Color.parseColor("#33333333"));
        paintRect.setAntiAlias(true);

        paintPoint = new Paint();
        paintPoint.setColor(Color.parseColor("#421AE6"));
        paintPoint.setAntiAlias(true);

        circleRadius = DensityUtils.dip2px(context, 8);
        setOnTouchListener(this);
    }

    public CropGraph(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CropGraph(Context context) {
        this(context, null);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                judgeTouchPosition(event.getX(), event.getY());
                Log.e("xhc", " x " + event.getX() + " y " + event.getY() + ltFlag + lbFlag + rtFlag + rbFlag);
                break;
            case MotionEvent.ACTION_MOVE:
                tempX = event.getX();
                tempY = event.getY();
                if (ltFlag) {
                    xRect = tempX;
                    yRect = tempY;
                    invalidate();
                } else if (rtFlag) {
                    invalidate();
                } else if (lbFlag) {
                    invalidate();
                } else if (rbFlag) {
                    invalidate();
                }
                break;
            case MotionEvent.ACTION_UP:
                ltFlag = false;
                lbFlag = false;
                rtFlag = false;
                rbFlag = false;
                break;
        }
        return true;
    }

    private void judgeTouchPosition(float x, float y) {
        if (x > (xRect - circleRadius) && x < (xRect + circleRadius) && y > (yRect - circleRadius) && y < (yRect + circleRadius)) {
            ltFlag = true;
        } else if (x > (xRect - circleRadius) && x < (xRect + circleRadius) && y > (yRect + heightRect - circleRadius) && y < (yRect + heightRect + circleRadius)) {
            lbFlag = true;
        } else if (x > (xRect + widthRect - circleRadius) && x < (xRect + widthRect + circleRadius) && y > (yRect - circleRadius) && y < (yRect + circleRadius)) {
            rtFlag = true;
        } else if (x > (xRect + widthRect - circleRadius) && x < (xRect + widthRect + circleRadius) && y > (yRect + heightRect - circleRadius) && y < (yRect + heightRect + circleRadius)) {
            rbFlag = true;
        }
    }

    @Override
    public void draw(Canvas canvas) {
        super.draw(canvas);
        drawRect(canvas);
        drawPoint(canvas);
    }


    private void drawRect(Canvas canvas) {
        rect.set(xRect, yRect, widthRect + xRect, heightRect + yRect);
        canvas.drawRect(rect, paintRect);
    }

    private void drawPoint(Canvas canvas) {
        canvas.drawCircle(xRect, yRect, circleRadius, paintPoint);
        canvas.drawCircle(xRect + widthRect, yRect, circleRadius, paintPoint);
        canvas.drawCircle(xRect, yRect + heightRect, circleRadius, paintPoint);
        canvas.drawCircle(xRect + widthRect, yRect + heightRect, circleRadius, paintPoint);
    }
}

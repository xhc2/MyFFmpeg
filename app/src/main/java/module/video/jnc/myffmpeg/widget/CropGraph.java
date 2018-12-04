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
    private float widthRect;
    private float heightRect;

    private float lastXRect;
    private float lastYRect;
    private float lastwidthRect;
    private float lastheightRect;

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
        circleRadius = DensityUtils.dip2px(context, 8);
        xRect = circleRadius;
        yRect = circleRadius;
        lastXRect = xRect;
        lastYRect = yRect;
        widthRect = minSize * 2;
        heightRect = minSize * 2;
        lastwidthRect = widthRect;
        lastheightRect = heightRect;

        rect = new RectF(xRect, yRect, widthRect + xRect, heightRect + yRect);
        paintRect = new Paint();
        paintRect.setColor(Color.parseColor("#55333333"));
        paintRect.setAntiAlias(true);

        paintPoint = new Paint();
        paintPoint.setColor(Color.parseColor("#421AE6"));
        paintPoint.setAntiAlias(true);
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
                break;
            case MotionEvent.ACTION_MOVE:
                tempX = event.getX();
                tempY = event.getY();
                if (ltFlag) {
                    if (widthRect - (tempX - lastXRect) >= minSize && (tempX - circleRadius) > 0) {
                        xRect = tempX;
                    }
                    if (heightRect - (tempY - lastYRect) >= minSize && (tempY - circleRadius) > 0) {
                        yRect = tempY;
                    }
                    widthRect -= (xRect - lastXRect);
                    heightRect -= (yRect - lastYRect);
                    lastXRect = xRect;
                    lastYRect = yRect;
                    invalidate();
                } else if (rtFlag) {
                    if ((tempX - xRect) >= minSize && (tempX + circleRadius) < getWidth()) {
                        widthRect -= lastwidthRect - (tempX - xRect);
                        lastwidthRect = widthRect;
                    }
                    if (heightRect - (tempY - lastYRect) >= minSize && (tempY - circleRadius) > 0) {
                        yRect = tempY;
                    }
                    heightRect -= (yRect - lastYRect);
                    lastXRect = xRect;
                    lastYRect = yRect;
                    invalidate();
                } else if (lbFlag) {
                    if (widthRect - (tempX - lastXRect) >= minSize && (tempX - circleRadius) > 0) {
                        xRect = tempX;
                    }
                    if (tempY - yRect >= minSize && (tempY + circleRadius) < getHeight()) {
                        heightRect -= lastheightRect - (tempY - yRect);
                        lastheightRect = heightRect;
                    }
                    widthRect -= (xRect - lastXRect);
                    lastXRect = xRect;
                    lastYRect = yRect;
                    invalidate();
                } else if (rbFlag) {
                    if ((tempX - xRect) >= minSize && (tempX + circleRadius) < getWidth()) {
                        widthRect -= lastwidthRect - (tempX - xRect);
                        lastwidthRect = widthRect;
                    }
                    if (tempY - yRect >= minSize && (tempY + circleRadius) < getHeight()) {
                        heightRect -= lastheightRect - (tempY - yRect);
                        lastheightRect = heightRect;
                    }
                    lastXRect = xRect;
                    lastYRect = yRect;
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

    public int[] getGraphResult() {
        int[] result = new int[4];
        result[0] = (int) xRect;
        result[1] = (int) yRect;
        result[2] = (int) widthRect;
        result[3] = (int) heightRect;
        return result;
    }

    private void drawRect(Canvas canvas) {
        rect.set(xRect, yRect, widthRect + xRect, heightRect + yRect);
        canvas.drawRect(rect, paintRect);
    }

    private void drawPoint(Canvas canvas) {
        canvas.drawCircle(xRect, yRect, circleRadius, paintPoint); //lt
        canvas.drawCircle(xRect + widthRect, yRect, circleRadius, paintPoint); // rt
        canvas.drawCircle(xRect, yRect + heightRect, circleRadius, paintPoint); //lb
        canvas.drawCircle(xRect + widthRect, yRect + heightRect, circleRadius, paintPoint); // rb
    }
}

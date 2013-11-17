int bounding_box_collision(int b1_x, int b1_y, int b1_w, int b1_h, int b2_x, int b2_y, int b2_w, int b2_h)
{
    if ((b1_x > b2_x + b2_w - 1 ) || // is b1 on the right side of b2?
        (b1_y > b2_y + b2_h - 1) || // is b1 under b2?
        (b2_x > b1_x + b1_w  -1) || // is b2 on the right side of b1?
        (b2_y > b1_y + b1_h - 1))   // is b2 under b1?
    {
        return 0;
    }
    return 1;
}

int ball_wall_collision(int ballX, int ballY, int ballDx, int ballDy, int ballWidth, int ballHeight, int screenWidth, int screenHeight)
{
if ( (ballX + ballHeight) > screenWidth && ballDx > 0) { return 1; }
if ( (ballY + ballHeight) > screenHeight && ballDy > 0) { return 1; }
if ( ballY <= 0 && ballDy < 0) { return 1; }
if ( ballX <= 0 && ballDx < 0) { return 1; }
else { return 0; }

}

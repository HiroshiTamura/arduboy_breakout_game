#include "resource.h"
#include <Arduboy2.h>

Arduboy2 ab;

#define BALL_COUNT 8
const short SCALE = 128;


const short screenWidth = 128;
const short screenHeight = 64;

short paddlePosX = (screenWidth / 2 - paddleWidth / 2) * SCALE;
const short paddlePosY = (screenHeight - paddleHeight) * SCALE;
const short paddleSpeed = 24;

const short g = 2;


struct BallInfo{
    short ballPosX;
    short ballPosY;
    short ballSpeedX;
    short ballSpeedY;
    bool ballEnable;  
} ballInfo[BALL_COUNT];

struct PrevBallPos{
    short x;
    short y;
} prevBallPos[BALL_COUNT];

const short maxBallSpeedY = 2048;
const char speedScale = 16;

unsigned long score = 0;
//unsigned long timeCount = 0;


long isqrt(long x)
{
  long s, t;

  if (x <= 0) return 0;

  s = 1;  t = x;
  while (s < t) {  s <<= 1;  t >>= 1;  }
  do {
    t = s;
    s = (x / s + s) >> 1;
  } while (s < t);

  return t;
}


void DrawAllBlock(){
    //block
    for(unsigned char y = 0; y < blockArrayHeight; y++){
        for(unsigned char x = 0; x < blockArrayWidth; x++){
            if(blockArray[y][x]){
                ab.drawBitmap(x * blockWidth, y * blockHeight, block, blockWidth, blockHeight, 1);
            }
        }
    }
}

void DrawScreen(){
    //clear score area
    ab.fillRect(0, 0, 127, 7, 0);

    
    //clear preview ball
    for(unsigned char i = 0; i < BALL_COUNT; i++){
        //ab.drawBitmap(prevBallPos[i].x / SCALE - ballWidth / 2, prevBallPos[i].y / SCALE - ballWidth / 2, ball, ballWidth, ballHeight, 0);
        ab.drawBitmap((prevBallPos[i].x >> 7) - ballWidth / 2, (prevBallPos[i].y >> 7) - ballWidth / 2, ball, ballWidth, ballHeight, 0);
    }
  
    //paddle
    ab.fillRect(paddlePosX / SCALE, paddlePosY / SCALE, paddleWidth, paddleHeight, 0);
    ab.drawBitmap(paddlePosX / SCALE, paddlePosY / SCALE, paddle, paddleWidth, paddleHeight, 1);

    //ball
    for(unsigned char i = 0; i < BALL_COUNT; i++){
        if(ballInfo[i].ballEnable){
            //ab.drawBitmap(ballInfo[i].ballPosX / SCALE - ballWidth / 2, ballInfo[i].ballPosY / SCALE - ballWidth / 2, ball, ballWidth, ballHeight, 1);
            ab.drawBitmap((ballInfo[i].ballPosX >> 7) - ballWidth / 2, (ballInfo[i].ballPosY >> 7) - ballWidth / 2, ball, ballWidth, ballHeight, 1);
        }
    }

    
    //score
    unsigned char index;
    unsigned long tmpScore = score;
    char drawPosX = screenWidth - numberWidth;
    do{
        index = tmpScore % 10;
        tmpScore /= 10;
        ab.drawBitmap(drawPosX, 0, number[index], numberWidth, numberHeight, 1);
        drawPosX -= numberWidth;
    }while(tmpScore > 0);
}

void setup(){
    // put your setup code here, to run once:

    ab.begin();
    ab.setFrameRate(60);
    ab.clear();

    DrawAllBlock();
}

void HitJudgement(char i){
    if(!ballInfo[i].ballEnable){
        return;
    }
    
    //bottom
    if( ballInfo[i].ballPosY >= (screenHeight - 2) * SCALE){
        //hit the paddle
        if( ballInfo[i].ballPosX + ballWidth * SCALE / 2 >= paddlePosX && ballInfo[i].ballPosX <= (paddlePosX + paddleWidth * SCALE)){
            //score++;
            
            short x = (ballInfo[i].ballPosX + ballWidth * SCALE / 2) - paddlePosX - ((paddleWidth + ballWidth) * SCALE / 2);
            //x /= SCALE;
            //long xx = ballInfo[i].ballPosX * ballInfo[i].ballPosX;
            //long yy = ballInfo[i].ballPosY * ballInfo[i].ballPosY;
            //long sr = isqrt( xx + yy);
            
            if(x < 0){
                ballInfo[i].ballSpeedX -= (long)x * x >> 10;
                //ballInfo[i].ballSpeedY = -ballInfo[i].ballSpeedY + ((long)x * x >> 10);
            }
            else{
                ballInfo[i].ballSpeedX += (long)x * x >> 10;
               // ballInfo[i].ballSpeedY = -ballInfo[i].ballSpeedY + ((long)x * x >> 10);
            }
            //ballSpeedX += x / 16;
            //ballInfo[i].ballPosX += ballInfo[i].ballSpeedX / speedScale;
            
            //ballInfo[i].ballPosY = (screenHeight - 2) * SCALE - (ballInfo[i].ballPosY - (screenHeight - 2) * SCALE);
            ballInfo[i].ballSpeedY = -ballInfo[i].ballSpeedY + 16;// + ((long)x * x > 13);
            //ballInfo[i].ballPosY += ballInfo[i].ballSpeedY / speedScale;

        }
        else{
            ballInfo[i].ballEnable = false;
        }
    }
    
    
    
    if( ballInfo[i].ballPosX < 0 && ballInfo[i].ballSpeedX < 0){
        ballInfo[i].ballPosX = -ballInfo[i].ballPosX;
        ballInfo[i].ballSpeedX = -ballInfo[i].ballSpeedX;
    }
    
    if( ballInfo[i].ballPosX > screenWidth * SCALE && ballInfo[i].ballSpeedX > 0){
        ballInfo[i].ballPosX = screenWidth * SCALE - (ballInfo[i].ballPosX - screenWidth * SCALE);
        ballInfo[i].ballSpeedX = -ballInfo[i].ballSpeedX;
    }
    
    if( ballInfo[i].ballPosY < 0 && ballInfo[i].ballSpeedY < 0){
        ballInfo[i].ballPosY = -ballInfo[i].ballPosY;
        ballInfo[i].ballSpeedY = -ballInfo[i].ballSpeedY;
    }
    
}



void loop() {
    // put your main code here, to run repeatedly:
    if(!(ab.nextFrame()))
    {
        //count time
        //timeCount++;
      
        //refresh preview ball position
        for(unsigned char i = 0; i < BALL_COUNT; i++){
            prevBallPos[i].x = ballInfo[i].ballPosX;
            prevBallPos[i].y = ballInfo[i].ballPosY;
        }
      
        ab.pollButtons();

        //paddle
        if(ab.pressed(LEFT_BUTTON)){
            if(ab.pressed(A_BUTTON)){
                paddlePosX -= paddleSpeed << 1;
            }
            else{
                paddlePosX -= paddleSpeed;
            }
        }
        if(ab.pressed(RIGHT_BUTTON)){
            if(ab.pressed(A_BUTTON)){
                paddlePosX += paddleSpeed << 1 ;
            }
            else{
                paddlePosX += paddleSpeed;
            }
        }

        if(paddlePosX < 0){
            paddlePosX = 0;
        }
        if(paddlePosX + paddleWidth * SCALE > screenWidth * SCALE){
            paddlePosX = (screenWidth - paddleWidth) * SCALE;
        }

    
        for(char i = 0; i < BALL_COUNT; i++){
            //move the ball
            if(ballInfo[i].ballEnable){
                //ballInfo[i].ballPosX += ballInfo[i].ballSpeedX / speedScale;
                //ballInfo[i].ballPosY += ballInfo[i].ballSpeedY / speedScale;
                ballInfo[i].ballPosX += ballInfo[i].ballSpeedX >> 4;
                ballInfo[i].ballPosY += ballInfo[i].ballSpeedY >> 4;
    
                ballInfo[i].ballSpeedY += g;
                //if( ballInfo[i].ballSpeedY >= maxBallSpeedY){
                //    ballInfo[i].ballSpeedY = maxBallSpeedY;
                //}
            }

            //hit block
            unsigned char x, y;
            //x = ballInfo[i].ballPosX / SCALE / blockWidth;
            //y = ballInfo[i].ballPosY / SCALE / blockHeight;
            x = ballInfo[i].ballPosX >> 9;
            y = ballInfo[i].ballPosY >> 9;
            if( 0 <= x && x < blockArrayWidth &&
                0 <= y && y < blockArrayHeight &&
                blockArray[y][x] == 1){
                
                blockArray[y][x] = 0;
                score += 10;
                ab.fillRect(x * blockWidth, y * blockHeight, blockWidth, blockHeight, 0);

                //ball reflection
                unsigned char px, py;
                px = prevBallPos[i].x >> 9;
                py = prevBallPos[i].y >> 9;
                if( x - px == 1 || x - px == -1){
                    ballInfo[i].ballSpeedX = -ballInfo[i].ballSpeedX;
                }
                if( y - py == 1 || y - py == -1){
                    ballInfo[i].ballSpeedY = -ballInfo[i].ballSpeedY;
                }
            }
            
            //hit judgement
            HitJudgement(i);
        }



        //apear the ball
        if(ab.justPressed(B_BUTTON)){
        //if(true){
            char i;
            for(i = 0; i < BALL_COUNT; i++){
                if(!ballInfo[i].ballEnable){
                    break;
                }
            }
            if(i < BALL_COUNT){
                ballInfo[i].ballPosX = 0;
                ballInfo[i].ballPosY = screenHeight * SCALE * 1 / 3;

                ballInfo[i].ballSpeedX = random(0,128) + 128;
                ballInfo[i].ballSpeedY = -128;

                ballInfo[i].ballEnable = true;
            }
        }


        //ab.clear();
        DrawScreen();
        ab.display();

        
        return;
    }

}

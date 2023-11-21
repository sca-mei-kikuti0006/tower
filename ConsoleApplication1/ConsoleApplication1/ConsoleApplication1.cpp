#include "DxLib.h"
#include <iostream>
#include <random>
#include <vector>
#include<string>



struct Bullet {
    double x, y; // 弾の位置
    int handle;  // 弾の画像のハンドル
};

int count1 = 0;
bool guncre1 = false;
int count2 = 0;
bool guncre2 = false;
int count3 = 0;
bool guncre3 = false;

void ShotCount(std::vector<Bullet>& bullets, int& count, int& lastShotTime, const int shotInterval, const int sh, double x, double y, int Count) {
    int currentTime = GetNowCount(); // 現在の時刻を取得

    if (currentTime - lastShotTime >= shotInterval && count < Count) {
        Bullet newBullet;
        newBullet.x = x; // 弾の初期 x 座標
        newBullet.y = y; // 弾の初期 y 座標
        newBullet.handle = sh; // 弾の画像ハンドル
        bullets.push_back(newBullet); // 弾をコレクションに追加
        count++;
        lastShotTime = currentTime; // 現在の時刻を更新
    }
}

bool CheckCollision(const Bullet& bullet, double targetX, double targetY, int targetHandle) {
    int bulletWidth, bulletHeight;
    GetGraphSize(bullet.handle, &bulletWidth, &bulletHeight);

    int targetWidth, targetHeight;
    GetGraphSize(targetHandle, &targetWidth, &targetHeight);

    if (bullet.x + bulletWidth > targetX &&
        targetX + targetWidth > bullet.x &&
        bullet.y + bulletHeight > targetY &&
        targetY + targetHeight > bullet.y) {
        return true; // 当たっている
    }

    return false; // 当たっていない
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (DxLib_Init() == -1) {
        return -1;
    }

    std::vector<Bullet> bullets; // 弾のコレクション

    int gh = LoadGraph("Airplane.png");
    int tw = LoadGraph("rdesign.png");
    int ca = LoadGraph("castle.png");
    int sh = LoadGraph("Projectile.png");
    int gun = LoadGraph("war_taihou.png");
    int imgWidth, imgHeight;
    GetGraphSize(gh, &imgWidth, &imgHeight); // 画像のサイズを取得

    int x = 0;
    int y = 200;
    bool draw = false;
    int mouseX, mouseY;
    int twWidth, twHeight;
    GetGraphSize(tw, &twWidth, &twHeight);
    int lastShotTime = 0; // 前回の弾の発射時刻
    const int shotInterval = 1000; // 発射間隔（ミリ秒）
    bool ghdelete = false;
    const int fontSize = 24; // フォントサイズ
    const int textColor = GetColor(255, 255, 255); // 文字色を白に設定（RGB: 255, 255, 255）
    int coinCount = 5; // コインの初期枚数
    int lastCoinTime = 0; // 最後にコインが増えた時間
    const int coinIncreaseInterval = 1000; // コインが増える間隔（ミリ秒）
    int hp = 5;
    int time = 0;


    while (!ProcessMessage()) {
        ClearDrawScreen();
        DrawGraph(500, 150, ca, TRUE);
        DrawGraph(110, 310, tw, TRUE);
        DrawGraph(270, 200, tw, TRUE);
        DrawGraph(400, 310, tw, TRUE);
        DrawGraph(x, y, gh, TRUE); // 画像を描画
        DrawString(100, 420, "Coin:3", textColor);
        DrawString(260, 310, "Coin:6", textColor);
        DrawString(390, 420, "Coin:9", textColor);
        int currentTime = GetNowCount();
        if (currentTime - lastCoinTime >= coinIncreaseInterval) {
            time++;
            if (time % 2 == 0) {
                coinCount++;
            }
            lastCoinTime = currentTime;
        }
        DrawString(525, 300, ("My Coin:" + std::to_string(coinCount)).c_str(), textColor);
        DrawString(555, 150, ("Hp:" + std::to_string(hp)).c_str(), textColor);
        DrawString(555, 50, ("Time:" + std::to_string(time)).c_str(), textColor);

        //画像の移動
        if (x == 100 || x == 300)
        {
            y -= 2;
            gh = LoadGraph("Airplane_up.png");
            if (y == 100 || y == 200)
            {
                gh = LoadGraph("Airplane.png");
                x += 2;
            }
        }
        else if (x == 200)
        {
            y += 2;
            gh = LoadGraph("Airplane_down.png");
            if (y == 300)
            {
                gh = LoadGraph("Airplane.png");
                x += 2;
            }
        }
        else
        {
            x += 2;
        }


        if (x >= 500 || ghdelete) { // 画像が画面外に出たら新しい画像をロード
            if (x >= 500)
            {
                hp--;
            }
            DeleteGraph(gh); // 古い画像を削除
            gh = LoadGraph("Airplane.png"); // 新しい画像をロード
            GetGraphSize(gh, &imgWidth, &imgHeight); // 新しい画像のサイズを取得
            x = -imgWidth; // 画像の幅分左側に配置してループさせる
            y = 200;
            ghdelete = false;
        }

        if (GetMouseInput() & MOUSE_INPUT_LEFT)
        {
            GetMousePoint(&mouseX, &mouseY);

            if (mouseX >= 110 && mouseX <= 110 + twWidth &&
                mouseY >= 310 && mouseY <= 310 + twHeight && coinCount >= 3 && !guncre1 && !guncre2 && !guncre3)
            {
                guncre1 = true;
                coinCount -= 3;
            }
            if (mouseX >= 270 && mouseX <= 270 + twWidth &&
                mouseY >= 200 && mouseY <= 200 + twHeight && coinCount >= 6 && !guncre1 && !guncre2 && !guncre3)
            {
                guncre2 = true;
                coinCount -= 6;

            }
            if (mouseX >= 400 && mouseX <= 400 + twWidth &&
                mouseY >= 310 && mouseY <= 310 + twHeight && coinCount >= 9 && !guncre1 && !guncre2 && !guncre3)
            {
                guncre3 = true;
                coinCount -= 9;

            }
        }

        SetMouseDispFlag(TRUE);

        for (auto& bullet : bullets) {
            double dx = 1.0; // 弾の x 方向の速度
            double dy = -1.0; // 弾の y 方向の速度

            double speed = 1.0; // 移動速度
            bullet.x -= dx * speed;
            bullet.y += dy * speed;

            DrawGraph(static_cast<int>(bullet.x), static_cast<int>(bullet.y), bullet.handle, TRUE);
        }

        for (auto it = bullets.begin(); it != bullets.end(); /* increment in loop */) {
            auto& bullet = *it;
            // 弾の描画

            // 特定の画像との当たり判定を行う
            if (CheckCollision(bullet, x, y, gh)) {
                // 画像に当たった弾を削除
                it = bullets.erase(it);
                ghdelete = true;
            }
            else {
                ++it;
            }
        }

        if (guncre1)
        {
            DrawGraph(110, 310, gun, TRUE);
            ShotCount(bullets, count1, lastShotTime, shotInterval, sh, 90.0, 300.0, 3);
            if (count1 == 3)
            {

                guncre1 = false;
                count1 = 0;
            }

        }
        if (guncre2)
        {
            DrawGraph(270, 200, gun, TRUE);
            ShotCount(bullets, count2, lastShotTime, shotInterval, sh, 250.0, 190.0, 6);
            if (count2 == 6)
            {
                guncre2 = false;
                count2 = 0;
            }
        }
        if (guncre3)
        {
            DrawGraph(400, 310, gun, TRUE);
            ShotCount(bullets, count3, lastShotTime, shotInterval, sh, 380.0, 300.0, 9);
            if (count3 == 9)
            {
                guncre3 = false;
                count3 = 0;
            }
        }
        if (hp == 0) {

            break;
        }


        ScreenFlip();
    }

    while (!ProcessMessage())
    {
        ClearDrawScreen();
        DrawString(300, 200, "Game Over", GetColor(255, 0, 0)); // "Game Over" を表示
        DrawString(300, 150, ("CrealTime:" + std::to_string(time)).c_str(), GetColor(255, 255, 255));

        ScreenFlip();

    }

    DxLib_End();
    return 0;
}
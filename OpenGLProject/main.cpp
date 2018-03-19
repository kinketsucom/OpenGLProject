//  main.cpp
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <GL/glut.h>
#include <chrono>
#include <thread>
#include "Vector.h"//自前で用意したもの
#include <fstream>
#include <sstream>
#include <math.h>
#include <fstream>


using namespace std;

//画面描画の設定など
static GLfloat rot_y, rot_x;    /* 立方体の回転角度 */
static GLfloat bgn_y, bgn_x;    /* ドラッグ開始時の回転角度 */
static int mouse_x, mouse_y;    /* ドラッグ開始時のマウスの位置 */
static GLuint cubelist;            /* 立方体のリスト */
GLfloat move_size = 0.1;

//関数プロトタイプ宣言
float SL_k(int mesh_k, int step, float dot_k, float r_k,int delay_k);
float FjT_k(int mesh_k, float dot_k, float r, int T);

//計算フェーズに必要なもの
bool start_bool = false;
//float dot_k = 0;
float dot_k[640];
float r_k[640];
//float r_k = 0;
float delayf_k_part[640];
float delayf_k[640];


//境界パラメータ
//ここではパラメータ要素数が418です
static float meshpoint[418][3];//メッシュのnode番号,３軸
static int meshtriangle[640][3];//三角形番号,さん各駅をなすnode番号
static float boundary_sol[640][16000];//三角形番号,時間ステップ数
//static float mesh_point_center[640][3];//三角形番号,3軸
//static float mesh_point_center_norm[640][3];//三角形番号,三軸
static float mesh_size[640];
static float bc_u[640][16000];
static VECTOR3 mesh_k_center[640];
static VECTOR3 mesh_k_norm[640];

//波のパラメータ
//入射波は1-cos
static float wave_speed = 340;
int samplerate = 8000;
float size = 1.0f;
float pi = (float)acos(-1);
float del_t = 1.0f / samplerate;
float lambda = 10.0f *del_t;
float time_len = 2.0;
float f[16000 * 2];

//必要かと思ったもの
//static float step_duration = (float)18000000.0 / samplerate; //μ秒
std::chrono::system_clock::time_point  start, end_time, start_all, end_all; // 型は auto で可

ofstream outputfile("./Output/u_array.txt");


class Camera {
public:
	VECTOR3 position = { 0.25 ,0.25,-0.9};
	VECTOR3 viewpoint = { 0.25,0.25,0 };
	void showPos() {//デバッグ用関数
		printf("position:(%f,%f,%f)\n", position.x, position.y, position.z);
	}
	void showView() {//デバッグ用関数
		printf("viewpoint:(%f,%f,%f)\n", viewpoint.x, viewpoint.y, viewpoint.z);
	}
	void showDetail() {//デバッグ用関数
		printf("***show***\n");
		printf("position:(%f,%f,%f)\n", position.x, position.y, position.z);
		printf("viewpoint:(%f,%f,%f)\n", viewpoint.x, viewpoint.y, viewpoint.z);
	}
};

/*カメラオブジェクトのインスタンス化*/
Camera cam;

/*
* キーが押されたときの処理
*/
void key_func(unsigned char key, int x, int y)
{
	switch (toupper(key)) {
	case 0x1b:    /* ＥＳＣキー */
				  /* プログラムを終了 */

		outputfile.close();
		exit(0);
		break;

	case 'T':
		start_bool = !start_bool;
		if (start_bool) {
			cout << "start" << endl;
		}
		else {
			cout << "stop" << endl;
		}
		
		break;

	case 'P':    /* Pキー */
		cam.showDetail();
		break;
	}
}


/*
*    画面更新時の処理
*/
void display_func(void)
{
	/*　画面と、デプスバッファを消去 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* 現在の座標系を保存 */
	glPushMatrix();
	//    /* まず、座標系を画面の奥に移動 */
	//    glTranslatef(0.0, 0.0, -15.0);
	//
	//    /* 回転 */
	glRotatef(rot_x, 1.0, 0.0, 0.0);
	glRotatef(rot_y, 0.0, 1.0, 0.0);

	/* 立方体の描画 */
	glCallList(cubelist);

	/* 座標系をもとに戻す */
	glPopMatrix();

	/* ダブルバッファのバッファを入れ替え、画面を更新 */
	glutSwapBuffers();
}


/*
*    立方体のリストを作成
*/
GLuint make_cube(void)
{
	GLint list;

	/* 立方体の頂点 */
	static GLfloat vert[][4] = {
		{ 0.0,  0.0, 0.0 },
	{ 0.0,  0.5,  0.0 },
	{ 0.5, 0.5,  0.0 },
	{ 0.5, 0.0,  0.0 },
	{ 0.0,  0.0, 0.15 },
	{ 0.0,  0.5, 0.15 },
	{ 0.5, 0.5, 0.15 },
	{ 0.5, 0.0, 0.15 },
	};


	/*{
		{ 1.0,  1.0,  1.0 },
	{ -1.0,  1.0,  1.0 },
	{ -1.0, -1.0,  1.0 },
	{ 1.0, -1.0,  1.0 },
	{ 1.0,  1.0, -1.0 },
	{ -1.0,  1.0, -1.0 },
	{ -1.0, -1.0, -1.0 },
	{ 1.0, -1.0, -1.0 },
	};*/

	/* 立方体の面の色 */
	static GLfloat color[][4] = {
		{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 1.0, 1.0, 0.0 },
	{ 1.0, 0.0, 1.0, 0.0 },
	{ 1.0, 1.0, 0.0, 0.0 },
	};

	/* 新しいリスト番号を取得 */
	list = glGenLists(1);

	/* 新しいリストを作成 */
	glNewList(list, GL_COMPILE);

	/* 立方体を描く */
	glBegin(GL_QUADS);
	glColor3fv(color[0]);
	glVertex3fv(vert[0]);
	glVertex3fv(vert[1]);
	glVertex3fv(vert[2]);
	glVertex3fv(vert[3]);

	glColor3fv(color[1]);
	glVertex3fv(vert[4]);
	glVertex3fv(vert[5]);
	glVertex3fv(vert[6]);
	glVertex3fv(vert[7]);

	glColor3fv(color[2]);
	glVertex3fv(vert[0]);
	glVertex3fv(vert[1]);
	glVertex3fv(vert[5]);
	glVertex3fv(vert[4]);

	glColor3fv(color[3]);
	glVertex3fv(vert[2]);
	glVertex3fv(vert[3]);
	glVertex3fv(vert[7]);
	glVertex3fv(vert[6]);

	glColor3fv(color[4]);
	glVertex3fv(vert[3]);
	glVertex3fv(vert[0]);
	glVertex3fv(vert[4]);
	glVertex3fv(vert[7]);

	glColor3fv(color[5]);
	glVertex3fv(vert[1]);
	glVertex3fv(vert[2]);
	glVertex3fv(vert[6]);
	glVertex3fv(vert[5]);

	glEnd();


	/* リスト作成終了 */
	glEndList();

	return list;
}

/*
*    ウインドウサイズ更新時の処理
*/
void reshape_func(int width, int height)
{
	/* 表示範囲設定 */
	glViewport(0, 0, width, height);
	/* 投影方法設定 */
	glMatrixMode(GL_PROJECTION);
	/*座標変換行列の初期化 */
	glLoadIdentity();
	/* 投影範囲を設定 */
	glFrustum(-1.0, 1.0, -1.0, 1.0, 3.0, 10000.0); //void glFrustum(l, r, b, t, n ,f)

	glMatrixMode(GL_MODELVIEW);
}

/*
*    マウスクリック時の処理
*/
void mouse_func(int button, int stat, int x, int y)
{
	if (GLUT_DOWN == stat && GLUT_LEFT == button) {
		/* マウスの左ボタンが押されたら、    */
		/* マウス位置を保存する             */
		mouse_x = x;
		mouse_y = y;
		bgn_y = rot_y;
		bgn_x = rot_x;
	}
}

/*
*    マウスドラッグ時の処理
*/
void drag_func(int x, int y)
{
	/* 回転角度を求める */
	rot_x = (GLfloat)(y - mouse_y) + bgn_x;
	rot_y = (GLfloat)(x - mouse_x) + bgn_y;

	/* 回転角度が範囲内かチェック */
	if (90<rot_x) {
		rot_x = 90;
	}
	if (rot_x<-90) {
		rot_x = -90;
	}

	/* 画面の描き換えを要求 */
	glutPostRedisplay();
}



/*
*    特殊キーが押されたときの処理
*/
void skey_func(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_DOWN:    /* したキー *///ここの挙動は気をつける回転について考慮してない
		cam.position.z += move_size;//位置移動//
		cam.viewpoint.z += move_size;//視点移動//
		gluLookAt(0, 0, move_size,
			0, 0, move_size - 1,
			0, 1, 0);
		//            rot_x+=10;
		break;
	case GLUT_KEY_UP:    /* 上キー *///
		cam.position.z -= move_size;//位置移動//
		cam.viewpoint.z -= move_size;//視点移動//
		gluLookAt(0, 0, -move_size,
			0, 0, -move_size - 1,
			0, 1, 0);
		break;
	case GLUT_KEY_LEFT:    /* 左キー */
						   //            rot_y+=10;
		cam.position.x -= move_size;//位置移動
		cam.viewpoint.x -= move_size;//視点移動
		gluLookAt(-move_size, 0, 0,
			-move_size, 0, -1,
			0, 1, 0);
		break;
	case GLUT_KEY_RIGHT: /* 右キー */
						 //            rot_y-=10;
		cam.position.x += move_size;//位置移動
		cam.viewpoint.x += move_size;//視点移動
		gluLookAt(move_size, 0, 0,
			move_size, 0, -1,
			0, 1, 0);
		//            cam.showDetail();
		break;
	}

	/* 画面の描き換えを要求 */
	glutPostRedisplay();
}

void loop() {

		while (1) {
			if (start_bool) {
				start_all = std::chrono::system_clock::now();
				for (int i = 0; i < 16000; i++) {
					start = std::chrono::system_clock::now(); // 計測開始時間
					//cout << "step:" << i << endl;
					float u_array = 0;
					VECTOR3 position = cam.position;
					if (i % 60 == 0) {//位置情報更新
						//内点計算
						for (int k = 0; k < 640; k++) {//各メッシュに対する計算ループ
							dot_k[k] = (position - mesh_k_center[k]).Dot(mesh_k_norm[k]);
							r_k[k] = position.Distance(mesh_k_center[k]);
							//r_k = position.DummyDistance(mesh_k_center[k]);//ダミー
							delayf_k_part[k] = samplerate * r_k[k] / wave_speed;
							delayf_k[k] = i - delayf_k_part[k];
							int delay_k = (int)delayf_k[k];
							if (delay_k > 0) {
								//これが新しいやつ
								u_array += -SL_k(k, i, dot_k[k], r_k[k],delay_k);
							}
						}
					}else {//位置は同じ
						for (int k = 0; k < 640; k++) {//各メッシュに対する計算ループ
							//内点計算
							delayf_k[k] = i - delayf_k_part[k];
							int delay_k = (int)delayf_k[k];
							if (delay_k > 0) {
								//これが新しいやつ
								u_array += -SL_k(k, i, dot_k[k], r_k[k],delay_k);
							}
						}
					}

					//u_array += f[i];
					outputfile << u_array << endl;


					end_time = std::chrono::system_clock::now();  // 計測終了時間
					float elapsed = (float)std::chrono::duration_cast<std::chrono::microseconds>(end_time - start).count();
					//            if(elapsed>=step_duration){
					//                printf("*over*%1f [μs]\n",elapsed);
					//            }else{
					//                printf("%1f [μs]\n",elapsed);
					//                usleep(step_duration-elapsed-1);
					//            }
				}

				//テストケース
				start_bool = false;

				end_all = std::chrono::system_clock::now();  // 計測終了時間
				float elapsed_all = (float)std::chrono::duration_cast<std::chrono::milliseconds>(end_all - start_all).count(); //処理に要した時間をミリ秒に変換
				printf("all_end:%1f [ms]\n", elapsed_all);
			}
		}

}



float SL_k(int mesh_k, int step, float dot_k, float r_k,int delay_k) {//メッシュkについて計算する
	float result = 0.0f;
	int n = step;
	int m1 = delay_k;
	int m2 = delay_k-1;
	result = FjT_k(mesh_k, dot_k, r_k, (n - m1 + 1)) * bc_u[mesh_k][m1] + FjT_k(mesh_k, dot_k, r_k, (m2 - n + 1))*bc_u[mesh_k][m2];
	return result;
}

float FjT_k(int mesh_k, float dot_k, float r, int T) {//SecondLayer計算用
	float del_t = 1.0f / samplerate;
	float result = 0.0f;
	result = - dot_k * mesh_size[mesh_k] * T / (4.0f * pi * r*r*r);
	return result;
}


/*
*    main関数
*        glutを使ってウインドウを作るなどの処理をする
*/
int main(int argc, char *argv[])
{
	printf("データを読み込み\n");
	///////////////////ファイルの読み込み////////////////////
	//meshpoint
	std::ifstream fin(".\\Resource\\meshpoint.d");
	std::string str;
	if (!fin) {
		printf("meshpointファイルが存在しません");
		system("pause");
		return 1;
	}
	else {
		int node = 0;
		while (getline(fin, str, ' '))
		{
			if (str == "" || str == "\n") {//空文字と改行コードをはじく
				continue;
			}
			else {
				meshpoint[node / 3][node % 3] = (float)stod(str);
				node += 1;
			}
		}
	}

	std::stringstream strstream;
	strstream << fin.rdbuf();
	fin.close();


	//meshtriangle
	std::ifstream fin2(".\\Resource\\meshtriangle.d");
	if (!fin2) {
		printf("meshtriangleファイルが存在しません");
		system("pause");
		return 1;
	}
	else {
		int node = 0;
		while (getline(fin2, str, ' '))
		{
			if (str == "" || str == "\n") {//空文字と改行コードをはじく
				continue;
			}
			else {
				meshtriangle[node / 3][node % 3] = stoi(str) - 1;
				node += 1;
			}
		}
	}

	strstream << fin2.rdbuf();
	fin2.close();

	//boundary_sol
	//std::ifstream fin3( ".\\Resource\\boundary_sol.d" );
	//	if( !fin3 ){
	//		printf("boundary_solファイルが存在しません");
	//		system("pause");
	//		return 1;
	//	}else{
	//		int node = 0;
	//		while (getline(fin3, str,' ')){
	//			if(str == "" || str == "\n"){//空文字と改行コードをはじく
	//				continue;
	//			}else{
	//				int log = node%1024000;
	//				if(log ==0){
	//					std::cout << std::to_string(node/102400) << "%" << std::endl;
	//				}
	//				//640要素でループ
	//				boundary_sol[node%640][node/640] = (float)stod(str); //node,step
	//				bc_u[node % 640][node / 640] = boundary_sol[node % 640][node / 640];
	//				node += 1;
	//			}
	//		}
	//	}
	//strstream << fin3.rdbuf();
	//fin3.close();
	//
	cout << "読み込み終了" << endl;


	////////////////////ファイル読み込み終了////////////////////





	////////////////////重心の位置、法線ベクトルの計算////////////////////
	for (int k = 0; k<640; k++) {//重心中心取得
		VECTOR3 point0 = { meshpoint[meshtriangle[k][0]][0],meshpoint[meshtriangle[k][0]][1],meshpoint[meshtriangle[k][0]][2] };
		VECTOR3 point1 = { meshpoint[meshtriangle[k][1]][0],meshpoint[meshtriangle[k][1]][1],meshpoint[meshtriangle[k][1]][2] };
		VECTOR3 point2 = { meshpoint[meshtriangle[k][2]][0],meshpoint[meshtriangle[k][2]][1],meshpoint[meshtriangle[k][2]][2] };
		//重心取得
		//mesh_point_center[k][0] = (point1.x + point2.x + point0.x) / 3;
		//mesh_point_center[k][1] = (point1.y + point2.y + point0.y) / 3;
		//mesh_point_center[k][2] = (point1.z + point2.z + point0.z) / 3;
		mesh_k_center[k] = (point0 + point1 + point2) / 3;
		//メッシュの面積計算
		VECTOR3 point0_1 = point1 - point0;
		VECTOR3 point0_2 = point2 - point0;
		mesh_size[k] = (point0_1.Cross(point0_2)).Magnitude() / 2;
		//法線ベクトル(ちょっと未確認)
		VECTOR3 norm = point0_1.Cross(point0_2) / (point0_1.Cross(point0_2)).Magnitude();
		VECTOR3 zero = { 0,0,0 };
		mesh_k_norm[k] = zero-norm;//プラスマイナス反転
	}
	////////////////////重心の位置、法線ベクトルの計算終了////////////////////
	///////////////////////入射波の計算///////////////////////////
	for (int t = 0; t < samplerate*(int)time_len; t++) {
		//			if (t < Static.samplerate * lambda) {
		f[t] = 1 - cos(2 * pi / lambda * t / samplerate);
		//			} else {
		//				Static.f [t] = 0;
		//			}
	}
	///////////////////////入射波の計算終了///////////////////////////

	std::thread t1(loop);
	/* glutの初期化 */
	glutInit(&argc, argv);

	/* 画面表示の設定 */
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	/* ウインドウの初期サイズを指定 */
	glutInitWindowSize(300, 300);

	/* ウインドウを作る */
	glutCreateWindow("glut window");

	/*初期視点の設定*/
	gluLookAt(cam.position.x, cam.position.y, cam.position.z, cam.viewpoint.x, cam.viewpoint.y, cam.viewpoint.z, 0, 1, 0);

	/* 画面更新用の関数を登録 */
	glutDisplayFunc(display_func);

	/* ウインドウのサイズ変更時の関数を登録 */
	glutReshapeFunc(reshape_func);

	/* キーボード入力用関数を登録 */
	glutKeyboardFunc(key_func);
	glutSpecialFunc(skey_func);

	/* マウス用関数を登録 */
	glutMotionFunc(drag_func);
	glutMouseFunc(mouse_func);

	/* デプスバッファを使うように設定 */
	glEnable(GL_DEPTH_TEST);

	/* リストを作成 */
	cubelist = make_cube();
	/* イベント処理などを始める */
	glutMainLoop();

	return 0;
}

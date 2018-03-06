//  main.cpp
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <GL/glut.h>
#include <chrono>
#include <thread>
#include "Vector.h"//���O�ŗp�ӂ�������
#include <fstream>
#include <sstream>

using namespace std;

static GLfloat rot_y, rot_x;    /* �����̂̉�]�p�x */
static GLfloat bgn_y, bgn_x;    /* �h���b�O�J�n���̉�]�p�x */
static int mouse_x, mouse_y;    /* �h���b�O�J�n���̃}�E�X�̈ʒu */
static GLuint cubelist;            /* �����̂̃��X�g */
GLfloat move_size = 1.0;

static int samplerate = 80;
static float step_duration = 1000000.0 / samplerate; //�ʕb
std::chrono::system_clock::time_point  start, end_time, start_all, end_all; // �^�� auto �ŉ�


																			//���E�p�����[�^
																			//�����ł̓p�����[�^�v�f����418�ł�
static float meshpoint[418][3];//���b�V����node�ԍ�,�R��
static int meshtriangle[640][3];//�O�p�`�ԍ�,����e�w���Ȃ�node�ԍ�
static float boundary_sol[640][16000];//�O�p�`�ԍ�,���ԃX�e�b�v��
static float mesh_point_center[640][3];//�O�p�`�ԍ�,3��
static float mesh_point_center_norm[640][3];//�O�p�`�ԍ�,�O��
static float mesh_size[640];




class Camera {
public:
	VECTOR3 position = { 0,0,20 };
	VECTOR3 viewpoint = { 0,0,0 };
	void showPos() {//�f�o�b�O�p�֐�
		printf("position:(%f,%f,%f)\n", position.x, position.y, position.z);
	}
	void showView() {//�f�o�b�O�p�֐�
		printf("viewpoint:(%f,%f,%f)\n", viewpoint.x, viewpoint.y, viewpoint.z);
	}
	void showDetail() {//�f�o�b�O�p�֐�
		printf("***show***\n");
		printf("position:(%f,%f,%f)\n", position.x, position.y, position.z);
		printf("viewpoint:(%f,%f,%f)\n", viewpoint.x, viewpoint.y, viewpoint.z);
	}
};

/*�J�����I�u�W�F�N�g�̃C���X�^���X��*/
Camera cam;

/*
*    ��ʍX�V���̏���
*/
void display_func(void)
{
	/*�@��ʂƁA�f�v�X�o�b�t�@������ */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* ���݂̍��W�n��ۑ� */
	glPushMatrix();
	//    /* �܂��A���W�n����ʂ̉��Ɉړ� */
	//    glTranslatef(0.0, 0.0, -15.0);
	//
	//    /* ��] */
	glRotatef(rot_x, 1.0, 0.0, 0.0);
	glRotatef(rot_y, 0.0, 1.0, 0.0);

	/* �����̂̕`�� */
	glCallList(cubelist);

	/* ���W�n�����Ƃɖ߂� */
	glPopMatrix();

	/* �_�u���o�b�t�@�̃o�b�t�@�����ւ��A��ʂ��X�V */
	glutSwapBuffers();
}


/*
*    �����̂̃��X�g���쐬
*/
GLuint make_cube(void)
{
	GLint list;

	/* �����̂̒��_ */
	static GLfloat vert[][4] = {
		{ 1.0,  1.0,  1.0 },
	{ -1.0,  1.0,  1.0 },
	{ -1.0, -1.0,  1.0 },
	{ 1.0, -1.0,  1.0 },
	{ 1.0,  1.0, -1.0 },
	{ -1.0,  1.0, -1.0 },
	{ -1.0, -1.0, -1.0 },
	{ 1.0, -1.0, -1.0 },
	};

	/* �����̖̂ʂ̐F */
	static GLfloat color[][4] = {
		{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 1.0, 1.0, 0.0 },
	{ 1.0, 0.0, 1.0, 0.0 },
	{ 1.0, 1.0, 0.0, 0.0 },
	};

	/* �V�������X�g�ԍ����擾 */
	list = glGenLists(1);

	/* �V�������X�g���쐬 */
	glNewList(list, GL_COMPILE);

	/* �����̂�`�� */
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


	/* ���X�g�쐬�I�� */
	glEndList();

	return list;
}

/*
*    �E�C���h�E�T�C�Y�X�V���̏���
*/
void reshape_func(int width, int height)
{
	/* �\���͈͐ݒ� */
	glViewport(0, 0, width, height);
	/* ���e���@�ݒ� */
	glMatrixMode(GL_PROJECTION);
	/*���W�ϊ��s��̏����� */
	glLoadIdentity();
	/* ���e�͈͂�ݒ� */
	glFrustum(-1.0, 1.0, -1.0, 1.0, 3.0, 10000.0); //void glFrustum(l, r, b, t, n ,f)

	glMatrixMode(GL_MODELVIEW);
}

/*
*    �}�E�X�N���b�N���̏���
*/
void mouse_func(int button, int stat, int x, int y)
{
	if (GLUT_DOWN == stat && GLUT_LEFT == button) {
		/* �}�E�X�̍��{�^���������ꂽ��A    */
		/* �}�E�X�ʒu��ۑ�����             */
		mouse_x = x;
		mouse_y = y;
		bgn_y = rot_y;
		bgn_x = rot_x;
	}
}

/*
*    �}�E�X�h���b�O���̏���
*/
void drag_func(int x, int y)
{
	/* ��]�p�x�����߂� */
	rot_x = (GLfloat)(y - mouse_y) + bgn_x;
	rot_y = (GLfloat)(x - mouse_x) + bgn_y;

	/* ��]�p�x���͈͓����`�F�b�N */
	if (90<rot_x) {
		rot_x = 90;
	}
	if (rot_x<-90) {
		rot_x = -90;
	}

	/* ��ʂ̕`��������v�� */
	glutPostRedisplay();
}

/*
* �L�[�������ꂽ�Ƃ��̏���
*/
void key_func(unsigned char key, int x, int y)
{
	switch (toupper(key)) {
	case 0x1b:    /* �d�r�b�L�[ */
				  /* �v���O�������I�� */
		exit(0);
		break;
	case 'P':    /* P�L�[ */
		cam.showDetail();
		break;
	}
}

/*
*    ����L�[�������ꂽ�Ƃ��̏���
*/
void skey_func(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_DOWN:    /* �����L�[ *///�����̋����͋C�������]�ɂ��čl�����ĂȂ�
		cam.position.z += move_size;//�ʒu�ړ�//
		cam.viewpoint.z += move_size;//���_�ړ�//
		gluLookAt(0, 0, move_size,
			0, 0, move_size - 1,
			0, 1, 0);
		//            rot_x+=10;
		break;
	case GLUT_KEY_UP:    /* ��L�[ *///
		cam.position.z -= move_size;//�ʒu�ړ�//
		cam.viewpoint.z -= move_size;//���_�ړ�//
		gluLookAt(0, 0, -move_size,
			0, 0, -move_size - 1,
			0, 1, 0);
		break;
	case GLUT_KEY_LEFT:    /* ���L�[ */
						   //            rot_y+=10;
		cam.position.x -= move_size;//�ʒu�ړ�
		cam.viewpoint.x -= move_size;//���_�ړ�
		gluLookAt(-move_size, 0, 0,
			-move_size, 0, -1,
			0, 1, 0);
		break;
	case GLUT_KEY_RIGHT: /* �E�L�[ */
						 //            rot_y-=10;
		cam.position.x += move_size;//�ʒu�ړ�
		cam.viewpoint.x += move_size;//���_�ړ�
		gluLookAt(move_size, 0, 0,
			move_size, 0, -1,
			0, 1, 0);
		//            cam.showDetail();
		break;
	}

	/* ��ʂ̕`��������v�� */
	glutPostRedisplay();
}


void loop() {

	//    int j = 0;
	//    while(1){
	//        start = std::chrono::system_clock::now(); // �v���J�n����
	//        j += 1;
	////        printf("step:%d\n",j);
	////1�����[�v���x�����E�H
	//
	////        cam.showPos();
	//        end = std::chrono::system_clock::now();  // �v���I������
	//        float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count(); //�����ɗv�������Ԃ��~���b�ɕϊ�
	////        printf("%1f [ms]\n",elapsed);
	//    }

	if (true) {//�����̓X�^�[�g�����ꍇ���l���Ă���
		start_all = std::chrono::system_clock::now();
		for (int i = 0; i<8000; i++) {
			start = std::chrono::system_clock::now(); // �v���J�n����
			printf("step%d\n", i);
			for (int k = 0; k<30000; k++) {//�O�����[�v�����E?
										   //������ӂœ��_�v�Z������
			}
			end_time = std::chrono::system_clock::now();  // �v���I������
			float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start).count();
			//            if(elapsed>=step_duration){
			//                printf("*over*%1f [��s]\n",elapsed);
			//            }else{
			//                printf("%1f [��s]\n",elapsed);
			//                usleep(step_duration-elapsed-1);
			//            }
		}
		end_all = std::chrono::system_clock::now();  // �v���I������
		float elapsed_all = std::chrono::duration_cast<std::chrono::milliseconds>(end_all - start_all).count(); //�����ɗv�������Ԃ��~���b�ɕϊ�
		printf("all_end:%1f [ms]\n", elapsed_all);

	}

}


/*
*    main�֐�
*        glut���g���ăE�C���h�E�����Ȃǂ̏���������
*/
int main(int argc, char *argv[])
{
	cout << "hogehogehogheo" << endl;

	printf("�f�[�^��ǂݍ���\n");
	///////////////////�t�@�C���̓ǂݍ���////////////////////
	//meshpoint
	std::ifstream fin("meshpoint.d");
	std::string str;
	if (!fin) {
		printf("meshpoint�t�@�C�������݂��܂���");
		system("pause");
		return 1;
	}
	else {
		int node = 0;
		while (getline(fin, str, ' '))
		{
			if (str == "" || str == "\n") {//�󕶎��Ɖ��s�R�[�h���͂���
				continue;
			}
			else {
				meshpoint[node / 3][node % 3] = stod(str);
				node += 1;
			}
		}
	}

	//meshtriangle
	std::ifstream fin2("meshtriangle.d");
	if (!fin2) {
		printf("meshtriangle�t�@�C�������݂��܂���");
		system("pause");
		return 1;
	}
	else {
		int node = 0;
		while (getline(fin2, str, ' '))
		{
			if (str == "" || str == "\n") {//�󕶎��Ɖ��s�R�[�h���͂���
				continue;
			}
			else {
				meshtriangle[node / 3][node % 3] = stoi(str) - 1;
				node += 1;
			}
		}
	}
	/*
	//boundary_sol
	std::ifstream fin3( "boundary_sol.d" );
	if( !fin3 ){
	printf("boundary_sol�t�@�C�������݂��܂���");
	system("pause");
	return 1;
	}else{
	int node = 0;
	while (getline(fin3, str,' '))
	{
	if(str == "" || str == "\n"){//�󕶎��Ɖ��s�R�[�h���͂���
	continue;
	}else{
	int log = node%1024000;
	if(log ==0){
	std::cout << std::to_string(node/102400) << "%" << std::endl;
	}
	//640�v�f�Ń��[�v
	boundary_sol[node%640][node/640] = stoi(str); //node,step
	node += 1;
	}
	}
	}
	strstream << fin3.rdbuf();
	fin3.close();
	*/
	std::stringstream strstream;
	strstream << fin.rdbuf();
	fin.close();
	strstream << fin2.rdbuf();
	fin2.close();

	////////////////////�t�@�C���ǂݍ��ݏI��////////////////////

	////////////////////�d�S�̈ʒu�A�@���x�N�g���̌v�Z////////////////////
	for (int i = 0; i<640; i++) {//�d�S���S�擾
		VECTOR3 point0 = { meshpoint[meshtriangle[i][0]][0] , meshpoint[meshtriangle[i][0]][1],meshpoint[meshtriangle[i][0]][2] };
		VECTOR3 point1 = { meshpoint[meshtriangle[i][1]][0],meshpoint[meshtriangle[i][1]][1],meshpoint[meshtriangle[i][1]][2] };
		VECTOR3 point2 = { meshpoint[meshtriangle[i][2]][0],meshpoint[meshtriangle[i][2]][1],meshpoint[meshtriangle[i][2]][2] };
		//�d�S�擾
		mesh_point_center[i][0] = (point1.x + point2.x + point0.x) / 3;
		mesh_point_center[i][1] = (point1.y + point2.y + point0.y) / 3;
		mesh_point_center[i][2] = (point1.z + point2.z + point0.z) / 3;
		//���b�V���̖ʐόv�Z
		VECTOR3 point0_1 = point1 - point0;
		VECTOR3 point0_2 = point2 - point0;
		mesh_size[i] = (point0_1.Cross(point0_2)).Magnitude() / 2;
		//�@���x�N�g��(������Ɩ��m�F)
		VECTOR3 norm = point0_1.Cross(point0_2) / (point0_1.Cross(point0_2)).Magnitude();
		mesh_point_center_norm[i][0] = norm.x;
		mesh_point_center_norm[i][1] = norm.y;
		mesh_point_center_norm[i][2] = norm.z;
	}


	//    std::thread t1(loop);
	/* glut�̏����� */
	glutInit(&argc, argv);

	/* ��ʕ\���̐ݒ� */
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	/* �E�C���h�E�̏����T�C�Y���w�� */
	glutInitWindowSize(300, 300);

	/* �E�C���h�E����� */
	glutCreateWindow("glut window");

	/*�������_�̐ݒ�*/
	gluLookAt(cam.position.x, cam.position.y, cam.position.z, cam.viewpoint.x, cam.viewpoint.y, cam.viewpoint.z, 0, 1, 0);

	/* ��ʍX�V�p�̊֐���o�^ */
	glutDisplayFunc(display_func);

	/* �E�C���h�E�̃T�C�Y�ύX���̊֐���o�^ */
	glutReshapeFunc(reshape_func);

	/* �L�[�{�[�h���͗p�֐���o�^ */
	glutKeyboardFunc(key_func);
	glutSpecialFunc(skey_func);

	/* �}�E�X�p�֐���o�^ */
	glutMotionFunc(drag_func);
	glutMouseFunc(mouse_func);

	/* �f�v�X�o�b�t�@���g���悤�ɐݒ� */
	glEnable(GL_DEPTH_TEST);

	/* ���X�g���쐬 */
	cubelist = make_cube();
	/* �C�x���g�����Ȃǂ��n�߂� */
	glutMainLoop();

	return 0;
}

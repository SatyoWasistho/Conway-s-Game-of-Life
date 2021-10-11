#include <iostream>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include "shaderClass.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"

struct particle {
	int x, y, ncount;
	std::vector<particle> neighbors;
	particle(int _x = 0, int _y = 0)
		:x(_x), y(_y), ncount(0)
	{}
	void gen_neighbors() {
		neighbors.push_back(particle(x - 1, y - 1));
		neighbors.push_back(particle(x, y - 1));
		neighbors.push_back(particle(x + 1, y - 1));
		neighbors.push_back(particle(x - 1, y));
		neighbors.push_back(particle(x + 1, y));
		neighbors.push_back(particle(x - 1, y + 1));
		neighbors.push_back(particle(x, y + 1));
		neighbors.push_back(particle(x + 1, y + 1));
	}
	bool operator==(const particle& a) const
	{
		return (x == a.x && y == a.y);
	}
	void operator =(const particle& a)
	{
		x = a.x;
		y = a.y;
		ncount = a.ncount;
	}
	bool operator !=(const particle& a) const
	{
		return !(*this == a);
	}
	size_t operator()(const particle& pointToHash) const noexcept {
		size_t hash = pointToHash.x + 10 * pointToHash.y;
		return hash;
	};
};

namespace std {
	template<> struct hash<particle>
	{
		std::size_t operator()(const particle& p) const noexcept
		{
			return p(p);
		}
	};
}

int main() {

	glfwInit();//initialize GLFW

	//specify GLFW version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//declare window size
	const unsigned int WINDOW_W = 1920;
	const unsigned int WINDOW_H = 1080;

	std::string title = "Conway's Game of Life";
	GLFWwindow* window = glfwCreateWindow(WINDOW_W, WINDOW_H, title.c_str(), glfwGetPrimaryMonitor(), NULL);//create window

	//error checking
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//set focus to window
	glfwMakeContextCurrent(window);

	//load GLAD to initialize OpenGL
	gladLoadGL();

	//set viewport to window
	glViewport(0, 0, WINDOW_W, WINDOW_H);

	//create geometry
	
	std::unordered_set<particle> life;
	std::unordered_set<particle> lifeadj;
	std::unordered_set<particle> newlife;
	std::unordered_set<particle> changelist;
	std::unordered_set<particle> newchangelist;
	

	Shader def("default.vert", "default.frag");

	std::vector<GLfloat> verticies;
	verticies.push_back(0);
	std::vector<GLuint> indicies;
	indicies.push_back(0);


	VAO vao;
	vao.Bind();

	VBO vbo(&verticies[0], sizeof(verticies));
	EBO ebo(&indicies[0], sizeof(indicies));

	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	vbo.Unbind();
	vao.Unbind();

	GLuint uniID = glGetUniformLocation(def.ID, "scale");

	//hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	//input flags
	double mousex, mousey;
	bool leftclickflag = 0;
	bool rightclickflag = 0;
	bool spaceflag = 0;
	bool upflag = 0;
	bool downflag = 0;
	bool leftflag = 0;
	bool rightflag = 0;
	float zoom = 1;

	//game state
	bool gamestate = 0;

	//camera
	particle camera;

	//framerate limiter and display
	double previousTime = glfwGetTime();
	int frameCount = 0;
	std::string frameratedisplay;
	int frameratelimit = 60;

	bool frame1 = 0;

	//window loop
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();

		while (frameCount != 0 && frameCount / (currentTime - previousTime) > frameratelimit) {
			currentTime = glfwGetTime();
		}
		frameCount++;

		//input handler
		glfwPollEvents();

		upflag = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		downflag = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		leftflag = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		rightflag = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) zoom *= 1.1;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) zoom /= 1.1;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) return 0;

		//frame 1 check
		if (spaceflag == 0 && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			gamestate = !gamestate;
			frame1 = 1;
		}

		spaceflag = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

		//camera movement
		if (upflag) camera.y-=5;
		if (downflag) camera.y+=5;
		if (leftflag) camera.x-=5;
		if (rightflag) camera.x+=5;
		
		glfwGetCursorPos(window, &mousex, &mousey);
		if (!gamestate) {
			leftclickflag = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
			rightclickflag = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

			if (leftclickflag) {
				particle p(((2 * mousex - WINDOW_W) / zoom + WINDOW_W)/2 + camera.x, ((2 * mousey - WINDOW_H) / zoom + WINDOW_H) / 2 +camera.y);
				life.insert(p);
			}
			if (rightclickflag) {
				particle p(((2 * mousex - WINDOW_W) / zoom + WINDOW_W) / 2 + camera.x, ((2 * mousey - WINDOW_H) / zoom + WINDOW_H) / 2 + camera.y);
				life.erase(p);
			}

			if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
				life.clear();
			}
		}
		else {
			if (frame1 ) {
				changelist.clear();
				newlife = life;
				lifeadj.clear();
				for (particle p : life) {
					p.ncount = 0;
					for (int i = p.x - 1; i < p.x + 2; i++) {
						for (int j = p.y - 1; j < p.y + 2; j++) {
							particle n(i, j);
							if (n != p) {
								if (life.find(n) != life.end()) {
									p.ncount++;
								}
								else {
									if (lifeadj.find(n) != lifeadj.end()) {
										n = *lifeadj.find(n);
										lifeadj.erase(n);
									}
									n.ncount++;
									if (n.ncount < 8) lifeadj.insert(n);
								}
							}
						}
					}
					if (p.ncount > 3 || p.ncount < 2) {
						newlife.erase(p);
						for (int i = p.x - 1; i < p.x + 2; i++) {
							for (int j = p.y - 1; j < p.y + 2; j++) {
								particle n(i, j);
								changelist.insert(n);
							}
						}

					}
				}
				for (particle p : lifeadj) {
					if (p.ncount == 3) {
						newlife.insert(p);
						for (int i = p.x - 1; i < p.x + 2; i++) {
							for (int j = p.y - 1; j < p.y + 2; j++) {
								particle n(i, j);
								changelist.insert(n);
							}
						}
					}
				}

				life = newlife;
				frame1 = 0;
			}
			else {
				newlife = life;
				for (particle p : changelist) {
					p.ncount = 0;
					bool alive = 0;
					particle n;
					for (n.x = p.x - 1; n.x < p.x + 2; n.x++) {
						for (n.y = p.y - 1; n.y < p.y + 2; n.y++) {
							if (life.find(n) != life.end()) {
								if (n == p) alive = 1;
								else p.ncount++;
							}
						}
					}
					if (alive && (p.ncount > 3 || p.ncount < 2)) {
						newlife.erase(p);
						for (n.x = p.x - 1; n.x < p.x + 2; n.x++) {
							for (n.y = p.y - 1; n.y < p.y + 2; n.y++) {
								newchangelist.insert(n);
							}
						}
					}
					if (!alive && p.ncount == 3) {
						newlife.insert(p);
						for (n.x = p.x - 1; n.x < p.x + 2; n.x++) {
							for (n.y = p.y - 1; n.y < p.y + 2; n.y++) {
								newchangelist.insert(n);
							}
						}
					}
				}
				life = newlife;
				changelist = newchangelist;
				newchangelist.clear();
			}
			//gamestate = 0;
		}
		int i = 0;
		for (particle p : life) {
			float posx = int((2 * (p.x - camera.x) / float(WINDOW_W) - 1) * zoom * WINDOW_W) / float(WINDOW_W);
			float posy = int((2 * (-p.y + camera.y) / float(WINDOW_H) + 1) * zoom * WINDOW_H) / float(WINDOW_H);
			if (abs(posx) < 1 && abs(posy) < 1) {
				verticies.push_back(posx);
				verticies.push_back(posy);
				verticies.push_back(0);
				if (zoom > 1) {
					verticies.push_back(posx+2*(zoom)/float(WINDOW_W));
					verticies.push_back(posy);
					verticies.push_back(0);
					verticies.push_back(posx);
					verticies.push_back(posy + 2 * (zoom) / float(WINDOW_H));
					verticies.push_back(0);
					verticies.push_back(posx + 2 * (zoom) / float(WINDOW_W));
					verticies.push_back(posy + 2 * (zoom) / float(WINDOW_H));
					verticies.push_back(0);

					indicies.push_back(i);
					indicies.push_back(i + 1);
					indicies.push_back(i + 2);
					indicies.push_back(i + 1);
					indicies.push_back(i + 2);
					indicies.push_back(i + 3);
					i += 4;
				}
			}
		}
		particle p(((2 * mousex - WINDOW_W) / zoom + WINDOW_W) / 2 + camera.x, ((2 * mousey - WINDOW_H) / zoom + WINDOW_H) / 2 + camera.y);
		float posx = int((2 * (p.x - camera.x) / float(WINDOW_W) - 1) * zoom * WINDOW_W) / float(WINDOW_W);
		float posy = int((2 * (-p.y + camera.y) / float(WINDOW_H) + 1) * zoom * WINDOW_H) / float(WINDOW_H);
		if (abs(posx) < 1 && abs(posy) < 1) {
			verticies.push_back(posx);
			verticies.push_back(posy);
			verticies.push_back(1);
			if (zoom > 1) {
				verticies.push_back(posx + 2 * zoom / float(WINDOW_W));
				verticies.push_back(posy);
				verticies.push_back(1);
				verticies.push_back(posx);
				verticies.push_back(posy + 2 * zoom / float(WINDOW_H));
				verticies.push_back(1);
				verticies.push_back(posx + 2 * zoom / float(WINDOW_W));
				verticies.push_back(posy + 2 * zoom / float(WINDOW_H));
				verticies.push_back(1);

				indicies.push_back(i);
				indicies.push_back(i + 1);
				indicies.push_back(i + 2);
				indicies.push_back(i + 1);
				indicies.push_back(i + 2);
				indicies.push_back(i + 3);
				i += 4;
			}
		}
		
		

		//background color
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		//render geometry from VAO
		if (verticies.size() > 0) {
			vbo.getData(&verticies[0], verticies.size() * sizeof(GLfloat));

			vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

			vbo.Unbind();
			vao.Unbind();

			def.Activate();
			glUniform1f(uniID, 1.0f);
			vao.Bind();
			if (zoom <= 1) glDrawArrays(GL_POINTS, 0, verticies.size() / 3);
			else {
				ebo.getData(&indicies[0], indicies.size() * sizeof(GLuint));
				glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
			}
		}

		//refresh screen
		glfwSwapBuffers(window);
		verticies.clear();
		indicies.clear();

		//framerate display
		std::ostringstream fpsstream;
		if (currentTime - previousTime >= 1.0)
		{
			fpsstream << frameCount << " | Live Cells: " << life.size() << " | Active Cells: " << changelist.size();

			frameCount = 0;
			previousTime = currentTime;
			frameratedisplay = title + " | FPS: " + fpsstream.str();
			glfwSetWindowTitle(window, frameratedisplay.c_str());
		}
	}

	//destroy all objects
	glfwDestroyWindow(window);
	def.Delete();
	vao.Delete();
	vbo.Delete();

	//terminate glfw instance
	glfwTerminate();

	return 0;
}
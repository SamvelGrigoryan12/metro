#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

struct Vagon {
	Vec2 v;
	bool stop = false;
	int znak;
	Timer ins;
};

enum Type
{
	None,
	Floor,
	Rels,
	Wall,
	Stop
};

class MyApp : public App
{
    void load()
    {
		/*//if (vagonTimer.check()) {
			auto vagon1 = vagons.load("Vagon.json", 5000, 500);
			auto vagon2 = vagons.load("Vagon.json", -5000, -500);
			auto& data1 = vagons.data(vagon1);
			data1.v = Vec2(-500, 0);
			auto& data2 = vagons.data(vagon2);
			data2.v = Vec2(500, 0);

		//}*/
		map<Color, int> colorToType;
		colorToType[Color(255, 255, 255)] = None;
		colorToType[Color(0, 0, 255)] = Floor;
		colorToType[Color(255, 0, 0)] = Rels;
		colorToType[Color(0, 0, 0)] = Wall;
		colorToType[Color(0, 255, 0)] = Stop;

		gmap = loadMap("fon.png", colorToType);
		for (int x = 0; x < gmap.w; ++x)
		{
			for (int y = 0; y < gmap.h; ++y)
			{
				if (gmap.get(x, y) == Wall)
					walls.load("Wall.json", pixels(x, y));

				if (gmap.get(x, y) == Floor)
					floor.load("Pol.json", pixels(x, y));

				if (gmap.get(x, y) == Rels)
					rels.load("Rels.json", pixels(x, y));

				if (gmap.get(x, y) == Stop)
					stops.load("Rels.json", pixels(x, y));

			}
		}
		connect(playb, play);
		connect(exitb, close);
		fighter.anim.run("xodba", 0);
		metrov.anim.run("open", 1);
		metrov.anim.run("close", 2);
		vagonTimer.repeat(20);
		ptime.repeat(0.5);
		ztime.repeat(2);
    }

	Vec2 pixels(int x, int y)
	{
		Vec2 v;
		v.x = x * 200 - gmap.w * 200 / 2 + 200 / 2;
		v.y = y * 200 - gmap.h * 200 / 2 + 200 / 2;
		return v;
	}

	void play() {
		metrov.anim.pause(1);
		metrov.anim.resume(2);
		playb.hide();
		helpb.hide();
		saitb.hide();
		exitb.hide();

	}


    void process(Input input)
    {
        using namespace gamebase::InputKey;
			if (input.pressed(W)) {
				fighter.anim.resume(0);
				fv.y = fvn;
			}
			else if (!input.pressed(S)) {
				fv.y = 0;
			}
			if (input.pressed(A)) {
				fighter.anim.resume(0);
				fv.x = -fvn;
			}
			else if (!input.pressed(D)) {
				fv.x = 0;
			}
			if (input.pressed(D)) {
				fighter.anim.resume(0);
				fv.x = fvn;
			}
			else if (!input.pressed(A)) {
				fv.x = 0;
			}
			if (input.pressed(S)) {
				fighter.anim.resume(0);
				fv.y = -fvn;
			}
			else if (!input.pressed(W)) {
				fv.y = 0;
			}
			if (!input.pressed(W) && !input.pressed(A) && !input.pressed(D) && !input.pressed(S)) {
				fighter.anim.pause(0);
			}

		if ((input.pressed(MouseLeft) || input.pressed(Space)) && ptime.check() && game.selected() == 2) {
			auto pula = Puli.load("Pula.json");
			auto d = Vec2(25, -20);
			d.rotate(fighter.angle());
			pula.setPos(d+fighter.pos());
			Vec2 delta = pula.pos() - field.mousePos();
			float angle = 3.14 + delta.angle();
			pula.setAngle(angle);
			auto& v = Puli.data(pula);
			v = pula.pos() - field.mousePos();
			v = polarVec(1000, 3.14 + 3.14 /2 - atan2(v.x, v.y));
		}

    }

    void move()
    {
		if (game.selected() == 1) {
			metrov.anim.resume(1);
			if (metrov.anim.isEmpty(1)) {
				playb.show();
				helpb.show();
				saitb.show();
				exitb.show();
			}
			if (metrov.anim.isEmpty(2)) {
				metrov.anim.pause(2);
				game.select(2);
			}
		}
		if (game.selected() == 2) {

			{
				auto fb = around(fighter.width(), fighter.height(), fighter.pos());
				fb.move(fb.w / -2, 0);
				fb.scale(0, 0.8);
				if (!walls.find(fb).empty() && fv.x < 0)
				{
					fv.x = 0;
					auto st = walls.find(fb).back().box();
					fighter.setPos(st.r + fighter.width() / 2, fighter.pos().y);
				}
			}
			{
				auto fb = around(fighter.width(), fighter.height(), fighter.pos());
				fb.move(fb.w / 2, 0);
				fb.scale(0, 0.8);
				if (!walls.find(fb).empty() && fv.x > 0)
				{
					fv.x = 0;
					auto st = walls.find(fb).back().box();
					fighter.setPos(st.l - fighter.width() / 2, fighter.pos().y);
				}
			}
			{
				auto fb = around(fighter.width(), fighter.height(), fighter.pos());
				fb.move(0, fb.h / 2 + 1);
				fb.scale(0.8, 0);
				if (!walls.find(fb).empty() && fv.y > 0)
				{
					fv.y = 0;
					auto st = walls.find(fb).back().box();
					fighter.setPos(fighter.pos().x, st.b - fighter.height() / 2);
				}
			}
			{
				auto fb = around(fighter.width(), fighter.height(), fighter.pos());
				fb.move(0, -fb.h / 2 - 1);
				fb.scale(0.8, 0);
				if (!walls.find(fb).empty() && fv.y < 0)
				{
					fv.y = 0;
					auto st = walls.find(fb).back().box();
					fighter.setPos(fighter.pos().x, st.t + fighter.height() / 2);
				}
			}


			for (auto pula : Puli.all())
				for (auto wall : walls.find(pula))
				{
					if (pula.intersects(wall)) {
						Puli.remove(pula);
						break;
					}
				}
			
			if (vagonTimer.check()) {
				auto vagon1 = vagons.load("Vagon.json", 5000, 500);
				auto vagon2 = vagons.load("Vagon.json", -5000, -500);
				auto& data1 = vagons.data(vagon1);
				data1.v = Vec2(-500, 0);
				data1.ins.stop();
				data1.znak = -1;
				auto& data2 = vagons.data(vagon2);
				data2.v = Vec2(500, 0);
				data2.ins.stop();
				data2.znak = 1;
				
			}
			
			for (auto vagon : vagons.all()) {
				auto& data = vagons.data(vagon);
				if (fighter.intersects(vagon)) {
					fighter.move(data.v * timeDelta());
				}
				for (auto zombie : Zombies.all()) {
					if (zombie.intersects(vagon)) {
						zombie.move(data.v * timeDelta());
					}
				}
				if (data.znak == -1) {
					if (!stops.find(vagon.box()).empty()) {
						data.stop = true;
					}
					if (data.stop) {
						data.v.x += 25 * timeDelta();
						if (data.v.x >= 0) {
							data.ins.start();
							data.stop = false;
						}	
					}
					else if (data.ins.isRunning()) {
						if (data.ins.time() > 5) {
							data.ins.stop();
						}
					}
					else {
						if (data.v.x >= -500) {
							data.v.x -= 25 * timeDelta();
						}
					}
					
				}
				vagon.move(data.v * timeDelta());
			}


			for (auto pula : Puli.all()) {
				pula.move(Puli.data(pula)*timeDelta());
			}
			Vec2 delta = fighter.pos() - field.mousePos();
			float angle = 3.14 + delta.angle();
			fighter.setAngle(angle);


			//if (ztime.check()) {
			//	auto zombie = Zombies.load("Zombie.json", -field.width() / 2, randomInt(-field.height() / 2, field.height() / 2));
			//}
			for (auto zombie : Zombies.all()) {
				auto& v1 = Zombies.data(zombie);
				Vec2 delta = - fighter.pos() + zombie.pos();
				float angle = 3.14 + delta.angle();
				zombie.setAngle(angle);

				auto& v = Zombies.data(zombie);
				v = zombie.pos() - fighter.pos();
				v = polarVec(50, 3.14 + 3.14 / 2 - atan2(v.x, v.y));
				zombie.move(v * timeDelta());
				zombie.anim.run("zxodba");
						
				//if (zombie.intersects(fighter)) {
				//	game.select(1);
				//	metrov.anim.run("onpos");
				//	metrov.anim.resume(1);
				//	//go = 0;
				//}
				for (auto pula : Puli.all()) {
					if (pula.intersects(zombie)) {
						Puli.remove(pula);
						Zombies.remove(zombie);
						break;
					}
				}
				
			}
		}

		fighter.move(fv*timeDelta());
		field.setView(fighter.pos());
		
    }


	GameMap gmap;
	FromDesign(GameView, field);
	FromDesign(GameObj, fighter);
	FromDesign(GameObj, metrov);
	FromDesign(Button, playb);
	FromDesign(Button, helpb);
	FromDesign(Button, saitb);
	FromDesign(Button, exitb);
	FromDesign(Selector, game);
	LayerFromDesign(Vec2, Puli);
	LayerFromDesign(Vec2, Zombies);
	LayerFromDesign(Vagon, vagons);
	LayerFromDesign(void, floor);
	LayerFromDesign(void, rels);
	LayerFromDesign(void, walls);
	LayerFromDesign(void, stops);


	Timer vagonTimer;
	Timer ptime;
	Timer ptimen;
	Timer ztime;
	Timer ztimen;
	Vec2 fv;

	int fvn = 180;
};



int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("metroConfig.json");
    app.setDesign("Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
     return 0;
}

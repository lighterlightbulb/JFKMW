#pragma once

class Particle
{
public:
	uint_fast8_t spr_tile = 0;
	uint_fast8_t spr_size = 0x11;
	uint_fast8_t pal_props = 0;
	uint_fast8_t special_animation_type = 0;
	double spr_x = 0;
	double spr_y = 0;
	double spr_sx = 0;
	double spr_sy = 0;
	double spr_grav = 0;

	void draw()
	{
		spr_sy -= spr_grav;
		spr_x += spr_sx;
		spr_y += spr_sy;

		uint_fast16_t oam_index = 0;
		uint_fast16_t s_x = uint_fast16_t(spr_x);
		uint_fast16_t s_y = uint_fast16_t(spr_y);
		while (oam_index < 0x400)
		{
			if (RAM[0x200 + oam_index] == 0 && RAM[0x206 + oam_index] == 0) { //Empty OAM slot found
				break;
			}
			oam_index += 8;
		}
		RAM[0x200 + oam_index] = spr_tile;
		RAM[0x201 + oam_index] = spr_size;
		RAM[0x202 + oam_index] = s_x;
		RAM[0x203 + oam_index] = s_x >> 8;
		RAM[0x204 + oam_index] = s_y;
		RAM[0x205 + oam_index] = s_y >> 8;
		RAM[0x206 + oam_index] = pal_props;
		RAM[0x207 + oam_index] = 0;
	}
};
vector<Particle> particles;

void createParticle(uint_fast8_t t, uint_fast8_t size, uint_fast8_t prop, uint_fast8_t anim_type, double x, double y, double sx, double sy, double grav)
{
	particles.push_back(Particle{ t, size, prop, anim_type, x, y, sx, sy, grav });
}

void processParticles()
{
	for (int i = 0; i < particles.size(); i++)
	{
		Particle& b = particles[i];
		b.draw();

		if (b.spr_y < -64)
		{
			particles.erase(particles.begin() + i);
			i--;
		}
	}
}
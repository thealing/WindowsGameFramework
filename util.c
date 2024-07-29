#include "util.h"

Shape* move_shape(Shape* shape)
{
	static Shape* old_shape;

	if (old_shape != NULL)
	{
		shape_destroy(old_shape);
	}

	old_shape = shape;

	return shape;
}

Shape* create_rect_shape(Vector min, Vector max)
{
	return shape_create_polygon(4, (Vector[]){ { min.x, min.y }, { max.x, min.y }, { max.x, max.y }, { min.x, max.y } });
}

void set_texture_and_color(const Texture* texture, const Color* color)
{
	graphics_set_texture(texture);
	
	if (color == NULL)
	{
		graphics_set_color(&(Color){ 1, 1, 1, 1 });
	}
	else
	{
		graphics_set_color(color);
	}
}

void draw_physics_world(const Physics_World* world)
{
	set_texture_and_color(NULL, &(Color){ 0, 1, 0, 1 });

	for (const List_Node* body_node = world->body_list.first; body_node != NULL; body_node = body_node->next)
	{
		const Physics_Body* body = body_node->item;

		draw_physics_body(body);
	}

	set_texture_and_color(NULL, &(Color){ 0, 0, 1, 1 });

	for (const List_Node* joint_node = world->joint_list.first; joint_node != NULL; joint_node = joint_node->next)
	{
		const Physics_Joint* joint = joint_node->item;

		draw_physics_joint(joint);
	}
}

void draw_physics_body(const Physics_Body* body)
{
	for (const List_Node* collider_node = body->collider_list.first; collider_node != NULL; collider_node = collider_node->next)
	{
		const Physics_Collider* collider = collider_node->item;

		draw_physics_collider(collider);
	}
}

void draw_physics_collider(const Physics_Collider* collider)
{
	graphics_draw_shape(collider->world_shape, false);

	if (collider->world_shape->type == SHAPE_TYPE_CIRCLE)
	{
		const Circle* circle = &collider->world_shape->circle;

		double angle = collider->body->angle;

		graphics_draw_segment(&(Segment){ circle->center, vector_add(circle->center, vector_create(circle->radius * cos(angle), circle->radius * sin(angle))) }, false);
	}
}

void draw_physics_joint(const Physics_Joint* joint)
{
	graphics_draw_segment(&(Segment){ joint->world_anchor_1, joint->world_anchor_2 }, false);
}

Vector get_mouse_position()
{
	Vector mouse_position;

	window_get_mouse_position(&mouse_position.x, &mouse_position.y);

	return mouse_position;
}

#include "util.h"

int main()
{
	window_create(640, 360, "Example");
	
	while (window_is_open())
	{
		window_update();
		
		graphics_clear(&(Color){ 1, 0, 0, 1 });
		
		printf("%d\n",random());
		
		if (window_is_mouse_button_down())
		{
			graphics_set_color(&(Color){ 0, 1, 0, 1 });
			
			graphics_draw_segment(&(Segment){ -1, -1, 1, 1 }, false);
			
			graphics_draw_segment(&(Segment){ 1, -1, -1, 1 }, false);
		}
		
		window_display();
	}
	
	window_destroy();
	
	return 0;
}

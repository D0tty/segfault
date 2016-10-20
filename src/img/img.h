struct image {
	int w, h;
	int *data;
};


struct image* image_create(int w, int h);

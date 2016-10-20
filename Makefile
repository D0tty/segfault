all: nn images

neural_net nn:
	cd ./src/nn/;\
	make all;\

images:
	cd ./src/img/;\
	make all;\

rieadme help:
	cat README

clean cl:
	cd ./src/nn/;make cl; cd ../img/;make cl;

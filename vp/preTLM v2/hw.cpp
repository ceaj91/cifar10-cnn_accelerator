#ifndef HW_C
#define HW_C
#include "hw.hpp"

Hardware::Hardware(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, sc_event* do_dense, deque <hwdata_t>& images,deque <hwdata_t>& weigts0, deque <hwdata_t>& weigts1, deque <hwdata_t>& weigts2,
			deque <hwdata_t>& bias0, deque <hwdata_t>& bias1, deque <hwdata_t>& bias2):sc_module(name), do_conv(do_conv),
																									do_maxpool(do_maxpool),
																									do_dense(do_dense),
																									images(images),
																									weigts0(weigts0), weigts1(weigts1), weigts2(weigts2),
																									bias0(bias0), bias1(bias1), bias2(bias2)
{
	std::cout << "HW constructed" << std::endl;
	SC_THREAD(proc);
	conv[0]=new ConvLayer("ConvLayer_no_0", &do_conv[0], &do_maxpool[0], images, weigts0, bias0,3,3,32);
	conv[1]=new ConvLayer("ConvLayer_no_1", &do_conv[1], &do_maxpool[1], images, weigts1, bias1,3,32,32);
	conv[2]=new ConvLayer("ConvLayer_no_2", &do_conv[2], &do_maxpool[2], images, weigts2, bias2,3,32,64);

	cout<<"Conv Layers constructed"<<endl;

	maxpool[0] = new MaxPoolLayer("MaxPoolLayer_no_0", &do_conv[1], &do_maxpool[0], images, 32, 32);
	maxpool[1] = new MaxPoolLayer("MaxPoolLayer_no_1", &do_conv[2], &do_maxpool[1], images, 16, 32);
	//maxpool[2] = new MaxPoolLayer("MaxPoolLayer_no_2", &do_dense, &do_maxpool[2], &do_dense, images, 10, 64);
	// ######## NOTE za ubuduce
	maxpool[2] = new MaxPoolLayer("MaxPoolLayer_no_2", &do_conv[0], &do_maxpool[2], images, 8, 64);


	// Razlog zasto je u MaxPoolLayer_no_2 prosledjen do_conv[0] jeste sto je 
	// Poslednji max pool layer indikator da je HW zavrsio obradu slike i to je znak
	// Memoriji da moze da predje na narednu sliku koju je CPU trebao da pripremi u memoriji
	// za vrijeme HW obrade prve slike
	// Takav paralelizam na ovom nivou apstrakcije jos nije moguc s obzirom da je nasa apstrakcija memorije
	// Ustvari globalni vectori koji se prosledjuju u svaki objekat svih klasa

	cout<<"MaxPool Layers constructed"<<endl;

}

void Hardware::proc()
{
	// Razlog za 2 waita:
	// Prvi wait ce da se triggeruje kada memorija kaze conv0 layeru da pocne da radi
	// Drugi se triggeruje kada MaxPool 2 layer (poslednji layer) kaze da je zavrsio da obradom slike i u tom trenutku
	// Slika odlazi u CPU na finalnu obradu, dok je HW spreman da primi novu sliku na obradu (ako cem raditi pipeline onda
	// je i ranije bio spreman, detalji za sada)
	// Da bi se ovo resilo trebalo bi uvesti i do_dense event u MaxPool layere i da poslednji MaxPool layer okida taj event
	// Sto je za sada nepotrebno komplikovanje i ovaj metod iako sasav funkcionise
	wait(do_conv[0]);
	wait(do_conv[0]);
	(*do_dense).notify(SC_ZERO_TIME);
}




#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct asistan{
	char isim[30];
	char kisaltma[4];
	int lab, ders, diper;
	int yuk;
	int program[12][6];
};

struct sinav{
	char ders[50];
	char gun[10];
	char saat[6];
	int baslangicSaati;
	int sure;
	struct asistan** gorevliler;
	int gorevliSayisi;
};

void asistan_getisim(struct asistan* a, FILE *f){
	int ch, prevch, i=0, j;
	while ((ch = fgetc(f)) != ';'){ //Dosyadan okunan karakter ';' olana kadar devam eder. 
		if(i==0){
			a->kisaltma[0] = ch; //�lk harfi k�saltmaya ekler.
			j = 1;
		}
		if(prevch==' '){
			a->kisaltma[j] = ch; //E�er okunan karakter, bo�ultan sonra geliyorsa, k�saltmaya ekler.
			j++;     
		}
		a->isim[i] =ch; //Okunan karakteri, isime ekler.
		prevch = ch; //�nceki karakter atamas�
		i++;
	}
	a->isim[i] ='\0'; //son eleman
	a->kisaltma[j] = '\0'; //son eleman
}

void asistan_get(struct asistan* a, FILE *f){
	asistan_getisim(a, f); //Asistan�n isim ve kisaltma bilgisi al�n�yor.
	int ch, prevch, row, i, j, k;
	int temp[2]; //Birler ve onlar basama�� hesab� i�in kullan�lacak dizi de�i�keni
	row = 1;
	i = -2; j = 0;
	while ((ch = fgetc(f)) != EOF){
		if (row>=3 && row <= 14){
			if(prevch == ';' && ch == 'f'){ //Asistan�n bo� saatlerini saptayan yap�
				a->program[i][j]=0;
				j++;
			}
			else if(prevch == ';' && ch != 'f'){ //Asistan�n dolu saatlerini saptayan yap�
				a->program[i][j]=1;
				j++;
			}
		}
		if(ch=='\n'){ //Sonraki saat bilgileri i�in de�i�ken temizleyen yap�
			row++;
			i++;
			j = 0;
			k = 0;
		}
		else if(row > 14){//lab, ders ve diper atamalar�n� yapan yap�
			if(ch >= '0' && ch <= '9'){
				temp[k] = ch - 48;
				k++;
			}
			if(k==1){
				if(row==15) a->lab = temp[0];
				else if(row==16) a->ders = temp[0];
				else if(row==17) a->diper = temp[0];
			}
			else if(k==2){
				if(row==15) a->lab = (temp[0]*10)+temp[1];
				else if(row==16) a->ders = (temp[0]*10)+temp[1];
				else if(row==17) a->diper = (temp[0]*10)+temp[1];
			}
		}
		prevch = ch; 
	}
	a->yuk = a->lab + a->ders + a->diper; //Asistan�n y�k�n� temsil eden tamsay� de�i�ken
}

void program_get(struct sinav** sinavlar, FILE* f){
		int ch, row=1, i=0, j=0;
		while ((ch = fgetc(f)) != EOF){
		if(row != 1){
			sinavlar[j] = (struct sinav*)malloc(sizeof(struct sinav));//Bilgileri al�nacak s�nav i�in dinamik bellek y�netimi
			i=0;
			while ((ch) != ';'){//ilk s�yun bilgilerini(ders ad�) �eken d�ng�
				sinavlar[j]->ders[i] = ch;
				ch = fgetc(f);
				i++;
			}
			sinavlar[j]->ders[i] = '\0';
			ch = fgetc(f);
			i=0;
			while ((ch) != ';'){//ikinci s�tun bilgilerini �eken d�ng�
				sinavlar[j]->gun[i] = ch;
				ch = fgetc(f);
				i++;
			}
			sinavlar[j]->gun[i] = '\0';
			//saat bilgilerinin, yap�ya eklenmesi
			//Ba�lang�� saati 'integer' tipinde al�n�yor.
			sinavlar[j]->baslangicSaati = ((ch = fgetc(f))-48)*10;
			sinavlar[j]->saat[0]=ch;
			sinavlar[j]->baslangicSaati += (ch = fgetc(f))-48;
			//Ba�lang�� saati karakter dizisi �elkinde al�n�yor.
			sinavlar[j]->saat[1]=ch;
			sinavlar[j]->saat[2]=(ch = fgetc(f));
			sinavlar[j]->saat[3]=(ch = fgetc(f));
			sinavlar[j]->saat[4]=(ch = fgetc(f));
			sinavlar[j]->saat[5]='\0';
			ch = fgetc(f);
			ch = fgetc(f);
			i=0;
			//S�nav s�resi hesaplayan d�ng�
			while(ch!=';'){
				if(i==2){
					sinavlar[j]->sure = sinavlar[j]->sure*10;
					ch = fgetc(f);
				}
					
				else{
					sinavlar[j]->sure = ((ch)-48)*10;
				ch = fgetc(f);
				i++;
				sinavlar[j]->sure += (ch)-48;
				ch = fgetc(f);
				i++;
				}
			}
			sinavlar[j]->gorevliSayisi=0;//G�revli say�s�, sonradan atanmak �zere ilklendiriliyor.
		}
		//Bir sonraki s�nav bilgileri i�in sat�r sonuna gidiliyor
		while (((ch = fgetc(f)) != '\n'));
		if(ch == '\n' && row != 1){
			row++;
			j++;
		}
		else if(ch == '\n')
			row++;
	}
	
}

void program_yazdir(struct sinav** sinavlar, int r){
	int i ,j;
	for(i=0; i<r; i++){
		printf("\n");
		printf("%s\n",sinavlar[i]->ders);
		printf("%s\n",sinavlar[i]->gun);
		printf("saat %d\n",sinavlar[i]->baslangicSaati);
		printf("%d dakika\n",sinavlar[i]->sure);
		printf("musait gorevli sayisi : %d\n", sinavlar[i]->gorevliSayisi);
		for(j=0; j<sinavlar[i]->gorevliSayisi;j++)
			printf("%s, %s(%d)\n", sinavlar[i]->gorevliler[j]->isim, sinavlar[i]->gorevliler[j]->kisaltma, sinavlar[i]->gorevliler[j]->yuk);
	}
}

void asistan_insert(struct asistan** a, int length){
	//INSERTION SORT
	int i, j;
	struct asistan* key;
	for(i = 1; i<length; i++){
		key = a[i];
		j = i - 1;
		while (j>=0 && a[j][0].yuk > key->yuk){
			a[j + 1] = a[j];
			j = j - 1;
		}
		a[j + 1] = key;
	}
}

void program_atama_sapta(struct asistan ** asistanlar, struct sinav ** sinavlar, int asistnum, int row){
	int i, j, k, l, saatSure;
	for(i=0; i<row; i++){//S�navlar� s�rayla gezmesi i�in olu�turulan iterasyonlu d�ng�
		saatSure = (sinavlar[i]->sure / 61) + 1;//S�nav s�resi dakikadan saate yuvarlan�l�yor.
		for(j=0; j<asistnum; j++){//S�navlar� s�rayla gezmesi i�in olu�turulan iterasyonlu d�ng�
			l=0;//�lgili asistan�n, ilgili derste bo� olup olmad���n� saptayan tamsay� de�i�keni
			for(k=0; k<saatSure; k++){//S�nav saati kadar gezmesi i�in olu�turulan iterasyonlu d�ng�
				if(strcmp(sinavlar[i]->gun,"Pazartesi")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][0]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Sali")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][1]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Carsamba")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][2]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Persembe")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][3]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Cuma")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][4]==0){
					l++;
				}
			}
			if(l==k){//E�er ilgili asistan, ilgili s�nav�n t�m saatlerinde bo� ise �al��acak kodu i�eren ko�ul
				sinavlar[i]->gorevliSayisi++;
			}
		}
		//S�navda yer alacak asistan say�s�na g�re yap�lan dinamik bellek y�netimi
		sinavlar[i]->gorevliler = (struct asistan **)malloc(sizeof(struct asistan)*sinavlar[i]->gorevliSayisi);
	}
}
 
void program_atama_yap(struct asistan ** asistanlar, struct sinav ** sinavlar, int asistnum, int row){
	int i, j, k, l, gs, saatSure;
	for(i=0; i<row; i++){//S�navlar� s�rayla gezmesi i�in olu�turulan iterasyonlu d�ng�
		gs=0;//G�revli say�s�
		saatSure = (sinavlar[i]->sure / 61) + 1;//S�nav s�resi dakikadan saate yuvarlan�l�yor.
		for(j=0; j<asistnum; j++){//S�navlar� s�rayla gezmesi i�in olu�turulan iterasyonlu d�ng�
			l=0;//�lgili asistan�n, ilgili derste bo� olup olmad���n� saptayan tamsay� de�i�keni
			for(k=0; k<saatSure; k++){//S�nav saati kadar gezmesi i�in olu�turulan iterasyonlu d�ng�
				if(strcmp(sinavlar[i]->gun,"Pazartesi")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][0]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Sali")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][1]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Carsamba")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][2]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Persembe")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][3]==0){
					l++;
				}
				else if(strcmp(sinavlar[i]->gun,"Cuma")==0 && asistanlar[j]->program[sinavlar[i]->baslangicSaati-8+k][4]==0){
					l++;
				}
			}
			if(l==k){//E�er ilgili asistan, ilgili s�nav�n t�m saatlerinde bo� ise �al��acak kodu i�eren ko�ul
				sinavlar[i]->gorevliler[gs] = asistanlar[j];//G�revli atamas�
				gs++;
			}
		}
	}
	
}
 
void program_dosyaya_yaz(FILE * fw, struct sinav ** sinavlar, struct asistan ** asistanlar, int asistnum, int row){
	int i, j;
	for(i=0; i<= row;i++){
		if(i==0){
			fprintf(fw, "SINAV;GUN;SAAT;SURE-DK;GOREVLILER;\n");//�lk sat�r
		}
		else{
			fprintf(fw, "%s;%s;%s;%d;", sinavlar[i-1]->ders, sinavlar[i-1]->gun, sinavlar[i-1]->saat, sinavlar[i-1]->sure);
			for(j=0; j<sinavlar[i-1]->gorevliSayisi; j++){
				if(j==0)
					fprintf(fw, "%s(%d)", sinavlar[i-1]->gorevliler[j]->kisaltma, sinavlar[i-1]->gorevliler[j]->yuk);
				else
					fprintf(fw, ",%s(%d)", sinavlar[i-1]->gorevliler[j]->kisaltma, sinavlar[i-1]->gorevliler[j]->yuk);	
			}
			fprintf(fw,";\n");
		}
	}
}
 
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) {
	FILE *fr, *fw; //Okunacak dosya ve yaz�lacak dosya i�in iki ayr� dosya i�aret�i tan�mlan�yor.
	int ch, row=1, asistnum=1, i;
	char **kisaltmalar;
	char* file_name; 
	file_name = (char*)malloc(256);
	struct asistan** asistanlar; 
	struct sinav** sinavlar; 
	printf("Yazdirilacak asistanlarin bulundugu dosya ismi(txt) : ");
	gets(file_name);
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	//Asistan say�s�n�n saptand��� d�ng�
	while ((ch = fgetc(fr)) != EOF){
		if(ch=='\n')
			asistnum++;
	}
	fclose(fr);
	kisaltmalar = (char**)malloc(4*asistnum); //Asistan k�saltmalar� i�in, asistan say�s� kadar yap�lan dinamik bellek y�netimi
	for(i=0;i<asistnum;i++) //Her bir asistan�n k�saltmas� i�in yap�lan dinamik bellek y�netimlerini i�eren d�ng�
		kisaltmalar[i] = (char*)malloc(4);
	asistanlar = (struct asistan**)malloc(sizeof(struct asistan)*asistnum); //Asistanlar i�in, asistan say�s� kadar yap�lan dinamik bellek y�netimi
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	i=0;
	// Asistan k�saltmalar�n� saptay�p atayan d�ng�
	while ((ch = fgetc(fr)) != EOF){
		// E�er bo�luk karakteri varsa, s�radaki iki karakteri k�saltmaya atayan d�ng�
		if(ch==' '){
			ch = fgetc(fr);
			kisaltmalar[i][0] = ch; 
			ch = fgetc(fr);
			kisaltmalar[i][1] = ch;
			ch = fgetc(fr);
			// K�saltman�n 2 ya da 3 karakter olma durumlar�na g�re k�saltmalara yap�lan atamalar� i�eren ko�ullar
			if(ch == '\n' ){
				kisaltmalar[i][2] = '\0';
				i++;
			}
			else{
				kisaltmalar[i][2]=ch;
				kisaltmalar[i][3]='\0';
				if(kisaltmalar[i][2]==-1)
					kisaltmalar[i][2] = '\0';
			}
		}
	}
	fclose(fr);
	// Asistanlar�n .csv dosyalar� i�in dosya ismi yap�land�rmalar�
	for(i = 0; i<asistnum; i++){
		// E�er k�saltma 2 karakterli ise dosya ismine yap�lacak atamalar
		if(kisaltmalar[i][2]=='\0'){
			file_name = (char*)malloc(7);
			file_name[0] = kisaltmalar[i][0];
			file_name[1] = kisaltmalar[i][1];
			file_name[2] = '\0';
			strcat(file_name, ".csv");
		}
		// E�er k�saltma 3 karakterli ise dosya ismine yap�lacak atamalar
		else{
			file_name = (char*)malloc(8);
			file_name[0] = kisaltmalar[i][0];
			file_name[1] = kisaltmalar[i][1];
			file_name[2] = kisaltmalar[i][2];
			file_name[3] = '\0';
			strcat(file_name, ".csv");
		}
		if ((fr = fopen(file_name, "r")) == NULL) {
			printf("cannot open file...\n");
			exit(EXIT_FAILURE);
		}
		asistanlar[i] = (struct asistan*)malloc(sizeof(struct asistan)); //Girilen her bir asistan i�in yap�lan dinamik bellek y�netimi
		asistan_get(asistanlar[i], fr); //Asistan bilgilerini, ilgili .csv dosyas�ndan �ekme i�lemi
		fclose(fr);
	}
	asistan_insert(asistanlar, asistnum); //Bilgisi girilen asistanlar� y�klerine g�re s�ralama i�lemi (insertion sort)
	//S�nav program� .csv dosyas� i�in bilgi giri�i
	printf("Yazdirilacak sinav programi dosyasinin ismi(csv) : ");
	gets(file_name);
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	//.csv uzant�l� dosyadaki sat�r say�s�n� saptama i�lemi
	while ((ch = fgetc(fr)) != EOF){
		if(ch=='\n')
			row++;
	}
	row=row-2;// S�nav say�s�
	fclose(fr);
	sinavlar = (struct sinav**)malloc(sizeof(struct sinav)*(row));//S�navlar i�in, hesaplanan s�nav say�s� kadar yap�lan dinamik bellek y�netimi
	//S�nav bilgilerini, ilgili .csv dosyas�ndan �ekme i�lemi
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	program_get(sinavlar, fr); //S�nav bilgilerini �eken i�lev
	program_atama_sapta(asistanlar, sinavlar, asistnum, row);//S�nav i�in atanacak g�revli say�s�n� belirleme i�lemi
	program_atama_yap(asistanlar, sinavlar, asistnum, row);//S�nav i�in g�revli atamas�n� yapma i�lemi
	program_yazdir(sinavlar, row); //S�navlar�n bilgilerini ekrana yazd�ran fonksiyon
	//Dosyaya yazmak i�in ba�lant� kurma i�lemi
	if ((fw = fopen(file_name, "w")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	program_dosyaya_yaz(fw, sinavlar, asistanlar, asistnum, row); //S�nav bilgilerini dosyaya yazma i�lemi
	fclose(fw);
	fclose(fr);
	return 0;
}

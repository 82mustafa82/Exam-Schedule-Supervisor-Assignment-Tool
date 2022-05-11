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
			a->kisaltma[0] = ch; //Ýlk harfi kýsaltmaya ekler.
			j = 1;
		}
		if(prevch==' '){
			a->kisaltma[j] = ch; //Eðer okunan karakter, boþultan sonra geliyorsa, kýsaltmaya ekler.
			j++;     
		}
		a->isim[i] =ch; //Okunan karakteri, isime ekler.
		prevch = ch; //Önceki karakter atamasý
		i++;
	}
	a->isim[i] ='\0'; //son eleman
	a->kisaltma[j] = '\0'; //son eleman
}

void asistan_get(struct asistan* a, FILE *f){
	asistan_getisim(a, f); //Asistanýn isim ve kisaltma bilgisi alýnýyor.
	int ch, prevch, row, i, j, k;
	int temp[2]; //Birler ve onlar basamaðý hesabý için kullanýlacak dizi deðiþkeni
	row = 1;
	i = -2; j = 0;
	while ((ch = fgetc(f)) != EOF){
		if (row>=3 && row <= 14){
			if(prevch == ';' && ch == 'f'){ //Asistanýn boþ saatlerini saptayan yapý
				a->program[i][j]=0;
				j++;
			}
			else if(prevch == ';' && ch != 'f'){ //Asistanýn dolu saatlerini saptayan yapý
				a->program[i][j]=1;
				j++;
			}
		}
		if(ch=='\n'){ //Sonraki saat bilgileri için deðiþken temizleyen yapý
			row++;
			i++;
			j = 0;
			k = 0;
		}
		else if(row > 14){//lab, ders ve diper atamalarýný yapan yapý
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
	a->yuk = a->lab + a->ders + a->diper; //Asistanýn yükünü temsil eden tamsayý deðiþken
}

void program_get(struct sinav** sinavlar, FILE* f){
		int ch, row=1, i=0, j=0;
		while ((ch = fgetc(f)) != EOF){
		if(row != 1){
			sinavlar[j] = (struct sinav*)malloc(sizeof(struct sinav));//Bilgileri alýnacak sýnav için dinamik bellek yönetimi
			i=0;
			while ((ch) != ';'){//ilk süyun bilgilerini(ders adý) çeken döngü
				sinavlar[j]->ders[i] = ch;
				ch = fgetc(f);
				i++;
			}
			sinavlar[j]->ders[i] = '\0';
			ch = fgetc(f);
			i=0;
			while ((ch) != ';'){//ikinci sütun bilgilerini çeken döngü
				sinavlar[j]->gun[i] = ch;
				ch = fgetc(f);
				i++;
			}
			sinavlar[j]->gun[i] = '\0';
			//saat bilgilerinin, yapýya eklenmesi
			//Baþlangýç saati 'integer' tipinde alýnýyor.
			sinavlar[j]->baslangicSaati = ((ch = fgetc(f))-48)*10;
			sinavlar[j]->saat[0]=ch;
			sinavlar[j]->baslangicSaati += (ch = fgetc(f))-48;
			//Baþlangýç saati karakter dizisi þelkinde alýnýyor.
			sinavlar[j]->saat[1]=ch;
			sinavlar[j]->saat[2]=(ch = fgetc(f));
			sinavlar[j]->saat[3]=(ch = fgetc(f));
			sinavlar[j]->saat[4]=(ch = fgetc(f));
			sinavlar[j]->saat[5]='\0';
			ch = fgetc(f);
			ch = fgetc(f);
			i=0;
			//Sýnav süresi hesaplayan döngü
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
			sinavlar[j]->gorevliSayisi=0;//Görevli sayýsý, sonradan atanmak üzere ilklendiriliyor.
		}
		//Bir sonraki sýnav bilgileri için satýr sonuna gidiliyor
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
	for(i=0; i<row; i++){//Sýnavlarý sýrayla gezmesi için oluþturulan iterasyonlu döngü
		saatSure = (sinavlar[i]->sure / 61) + 1;//Sýnav süresi dakikadan saate yuvarlanýlýyor.
		for(j=0; j<asistnum; j++){//Sýnavlarý sýrayla gezmesi için oluþturulan iterasyonlu döngü
			l=0;//Ýlgili asistanýn, ilgili derste boþ olup olmadýðýný saptayan tamsayý deðiþkeni
			for(k=0; k<saatSure; k++){//Sýnav saati kadar gezmesi için oluþturulan iterasyonlu döngü
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
			if(l==k){//Eðer ilgili asistan, ilgili sýnavýn tüm saatlerinde boþ ise çalýþacak kodu içeren koþul
				sinavlar[i]->gorevliSayisi++;
			}
		}
		//Sýnavda yer alacak asistan sayýsýna göre yapýlan dinamik bellek yönetimi
		sinavlar[i]->gorevliler = (struct asistan **)malloc(sizeof(struct asistan)*sinavlar[i]->gorevliSayisi);
	}
}
 
void program_atama_yap(struct asistan ** asistanlar, struct sinav ** sinavlar, int asistnum, int row){
	int i, j, k, l, gs, saatSure;
	for(i=0; i<row; i++){//Sýnavlarý sýrayla gezmesi için oluþturulan iterasyonlu döngü
		gs=0;//Görevli sayýsý
		saatSure = (sinavlar[i]->sure / 61) + 1;//Sýnav süresi dakikadan saate yuvarlanýlýyor.
		for(j=0; j<asistnum; j++){//Sýnavlarý sýrayla gezmesi için oluþturulan iterasyonlu döngü
			l=0;//Ýlgili asistanýn, ilgili derste boþ olup olmadýðýný saptayan tamsayý deðiþkeni
			for(k=0; k<saatSure; k++){//Sýnav saati kadar gezmesi için oluþturulan iterasyonlu döngü
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
			if(l==k){//Eðer ilgili asistan, ilgili sýnavýn tüm saatlerinde boþ ise çalýþacak kodu içeren koþul
				sinavlar[i]->gorevliler[gs] = asistanlar[j];//Görevli atamasý
				gs++;
			}
		}
	}
	
}
 
void program_dosyaya_yaz(FILE * fw, struct sinav ** sinavlar, struct asistan ** asistanlar, int asistnum, int row){
	int i, j;
	for(i=0; i<= row;i++){
		if(i==0){
			fprintf(fw, "SINAV;GUN;SAAT;SURE-DK;GOREVLILER;\n");//Ýlk satýr
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
	FILE *fr, *fw; //Okunacak dosya ve yazýlacak dosya için iki ayrý dosya iþaretçi tanýmlanýyor.
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
	//Asistan sayýsýnýn saptandýðý döngü
	while ((ch = fgetc(fr)) != EOF){
		if(ch=='\n')
			asistnum++;
	}
	fclose(fr);
	kisaltmalar = (char**)malloc(4*asistnum); //Asistan kýsaltmalarý için, asistan sayýsý kadar yapýlan dinamik bellek yönetimi
	for(i=0;i<asistnum;i++) //Her bir asistanýn kýsaltmasý için yapýlan dinamik bellek yönetimlerini içeren döngü
		kisaltmalar[i] = (char*)malloc(4);
	asistanlar = (struct asistan**)malloc(sizeof(struct asistan)*asistnum); //Asistanlar için, asistan sayýsý kadar yapýlan dinamik bellek yönetimi
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	i=0;
	// Asistan kýsaltmalarýný saptayýp atayan döngü
	while ((ch = fgetc(fr)) != EOF){
		// Eðer boþluk karakteri varsa, sýradaki iki karakteri kýsaltmaya atayan döngü
		if(ch==' '){
			ch = fgetc(fr);
			kisaltmalar[i][0] = ch; 
			ch = fgetc(fr);
			kisaltmalar[i][1] = ch;
			ch = fgetc(fr);
			// Kýsaltmanýn 2 ya da 3 karakter olma durumlarýna göre kýsaltmalara yapýlan atamalarý içeren koþullar
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
	// Asistanlarýn .csv dosyalarý için dosya ismi yapýlandýrmalarý
	for(i = 0; i<asistnum; i++){
		// Eðer kýsaltma 2 karakterli ise dosya ismine yapýlacak atamalar
		if(kisaltmalar[i][2]=='\0'){
			file_name = (char*)malloc(7);
			file_name[0] = kisaltmalar[i][0];
			file_name[1] = kisaltmalar[i][1];
			file_name[2] = '\0';
			strcat(file_name, ".csv");
		}
		// Eðer kýsaltma 3 karakterli ise dosya ismine yapýlacak atamalar
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
		asistanlar[i] = (struct asistan*)malloc(sizeof(struct asistan)); //Girilen her bir asistan için yapýlan dinamik bellek yönetimi
		asistan_get(asistanlar[i], fr); //Asistan bilgilerini, ilgili .csv dosyasýndan çekme iþlemi
		fclose(fr);
	}
	asistan_insert(asistanlar, asistnum); //Bilgisi girilen asistanlarý yüklerine göre sýralama iþlemi (insertion sort)
	//Sýnav programý .csv dosyasý için bilgi giriþi
	printf("Yazdirilacak sinav programi dosyasinin ismi(csv) : ");
	gets(file_name);
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	//.csv uzantýlý dosyadaki satýr sayýsýný saptama iþlemi
	while ((ch = fgetc(fr)) != EOF){
		if(ch=='\n')
			row++;
	}
	row=row-2;// Sýnav sayýsý
	fclose(fr);
	sinavlar = (struct sinav**)malloc(sizeof(struct sinav)*(row));//Sýnavlar için, hesaplanan sýnav sayýsý kadar yapýlan dinamik bellek yönetimi
	//Sýnav bilgilerini, ilgili .csv dosyasýndan çekme iþlemi
	if ((fr = fopen(file_name, "r")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	program_get(sinavlar, fr); //Sýnav bilgilerini çeken iþlev
	program_atama_sapta(asistanlar, sinavlar, asistnum, row);//Sýnav için atanacak görevli sayýsýný belirleme iþlemi
	program_atama_yap(asistanlar, sinavlar, asistnum, row);//Sýnav için görevli atamasýný yapma iþlemi
	program_yazdir(sinavlar, row); //Sýnavlarýn bilgilerini ekrana yazdýran fonksiyon
	//Dosyaya yazmak için baðlantý kurma iþlemi
	if ((fw = fopen(file_name, "w")) == NULL) {
		printf("cannot open file...\n");
		exit(EXIT_FAILURE);
	}
	program_dosyaya_yaz(fw, sinavlar, asistanlar, asistnum, row); //Sýnav bilgilerini dosyaya yazma iþlemi
	fclose(fw);
	fclose(fr);
	return 0;
}

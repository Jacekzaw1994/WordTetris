#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

const short maxSlowo = 10;
const short maxPionowo = 15;
const short maxPoziomo = 20;
const float czasNaRuch = 0.5;

enum
{
    LEWY = 97,
    PRAWY = 100,  
    SZYBKI = 115
};

struct Litera
{
    short x;
    short y;
    char znak;
};

void zerowaniePolaGry(char poleGry[maxPionowo][maxPoziomo])
{
    int i, j;
    for(j=0; j<maxPionowo; j++)
        for(i=0; i<maxPoziomo; i++)
            poleGry[j][i] = ' ';
}

void pokazPoleGry(char poleGry[maxPionowo][maxPoziomo])
{
    int i, j;

    for(i=0; i<maxPoziomo+2; i++)
    {
        printf("-");
    }
    printf("\n");

    for(j=0; j<maxPionowo; j++)
    {
        for(i=0; i<maxPoziomo; i++)
        {
            if( i == 0 ) printf("|");
            printf("%c", poleGry[j][i]);
            if( i == maxPoziomo-1 ) printf("|");
        }
        printf("\n");
    }

    for(i=0; i<maxPoziomo+2; i++)
    {
        printf("-");
    }
    printf("\n");
}

void pokazOknoGry(char poleGry[maxPionowo][maxPoziomo], int wynik)
{
    system("cls");
    printf("\nWynik: %d\n\n", wynik);
    pokazPoleGry(poleGry);
    printf("Szybko w dol: [s]\nWyjscie: [ESC]\n");
}

short szukajSlownik(char ** slownik, long iloscSlow, char * ciag, short dlugosc)
{
    // Binarne szukanie pierwszego znaku
    long s;
    long l = 0;
	long p = iloscSlow - 1;
	long znaleziony = -1;

	while(l <= p)
	{
		s = (l + p)/2;
		if(*(*(slownik + s)) == ciag[0])
		{
			znaleziony = s;
			break;
		}
		else if(*(*(slownik + s)) < ciag[0])
			l = s + 1;
		else
			p = s - 1;
	}

	if(znaleziony == -1) return 0;
    if(strcmp(*(slownik + znaleziony), ciag) == 0) return 1;

    // Cofa sie...
    if(ciag[1] <= *(*(slownik + znaleziony) + 1))
    {
        long i = znaleziony;
        while(i >= 0 && *(*(slownik + i)) == ciag[0])
        {
            if(strcmp(*(slownik + i), ciag) == 0) return 1;
            i--;
        }
    }

    // Idzie dalej...
    if(ciag[1] >= *(*(slownik + znaleziony) + 1))
    {
        long i = znaleziony;
        while(i < iloscSlow && *(*(slownik + i)) == ciag[0])
        {
            if(strcmp(*(slownik + i), ciag) == 0) return 1;
            i++;
        }
    }

    return 0;
}

void usunSlowoPionowe(char poleGry[maxPionowo][maxPoziomo], short x, short y, int dlugosc)
{
    int i;
    for(i=0; i<dlugosc; i++)
        poleGry[y + i][x] = ' ';
}

void usunSlowoPoziome(char poleGry[maxPionowo][maxPoziomo], short x, short y, int dlugosc)
{
    int i, j;
    for(i=0; i<dlugosc; i++)
    {
        for(j=y; (j>=0 && poleGry[j][x + i] != ' '); j--)
        {
            if( j > 0 )
                poleGry[j][x + i] = poleGry[j - 1][x + i];
            else
                poleGry[j][x + i] = ' ';
        }
    }
}

void szukajSlowa(struct Litera * aktywna, char poleGry[maxPionowo][maxPoziomo], int * wynik, char ** slownik, long iloscSlow)
{
    int dlugosc, i;

    // Poziomo
    char ciagPoziomy[aktywna->x + 1][maxSlowo + 1];
    short ciagPoziomyPowodzenia[aktywna->x + 1];
    for(i=0; i < aktywna->x + 1; i++) ciagPoziomyPowodzenia[i] = 0;

    short x;
    for(x=aktywna->x; (x >= 0 && poleGry[aktywna->y][x] != ' '); x--)
    // do tablicy odrotnie czyli [aktywna->x - x]
    {
        for(dlugosc = 0; (poleGry[aktywna->y][x + dlugosc] != ' ' && dlugosc < maxSlowo && x + dlugosc < maxPoziomo); dlugosc++)
            ciagPoziomy[aktywna->x - x][dlugosc] = poleGry[aktywna->y][x + dlugosc];
        ciagPoziomy[aktywna->x - x][dlugosc] = '\0';

        if(dlugosc >= 2 && x + dlugosc-1 >= aktywna->x)
        {
            do
            {
                ciagPoziomyPowodzenia[aktywna->x - x] = szukajSlownik(slownik, iloscSlow, ciagPoziomy[aktywna->x - x], dlugosc);
                if(!ciagPoziomyPowodzenia[aktywna->x - x])
                {
                    dlugosc--;
                    ciagPoziomy[aktywna->x - x][dlugosc] = '\0';
                }
            } while(!ciagPoziomyPowodzenia[aktywna->x - x] && dlugosc >= 2);
        }
    }

    short startX;
    short powodzenie = 0;
    dlugosc = 0;

    for(i=0; i < aktywna->x + 1; i++)
    {
        if(ciagPoziomyPowodzenia[i] == 1)
        {
            powodzenie = 1;
            int temp = strlen(ciagPoziomy[i]);
            if(temp > dlugosc)
            {
                startX = aktywna->x - i;
                dlugosc = temp;
            }
        }
    }

    if(powodzenie)
    {
        *wynik += dlugosc;
        usunSlowoPoziome(poleGry, startX, aktywna->y, dlugosc);
        Beep(750, 300);
    }
    else
    {
        // Pionowo
        char ciagPionowy[maxSlowo + 1];
        for(dlugosc = 0; (dlugosc < maxSlowo && dlugosc + aktywna->y < maxPionowo); dlugosc++)
            ciagPionowy[dlugosc] = poleGry[dlugosc + aktywna->y][aktywna->x];
        ciagPionowy[dlugosc] = '\0';

        powodzenie = 0;
        if(dlugosc >= 2)
        {
            do
            {
                powodzenie = szukajSlownik(slownik, iloscSlow, ciagPionowy, dlugosc);
                if(!powodzenie)
                {
                    dlugosc--;
                    ciagPionowy[dlugosc] = '\0';
                }
            } while(!powodzenie && dlugosc >= 2);

            if(powodzenie)
            {
                *wynik += dlugosc;
                usunSlowoPionowe(poleGry, aktywna->x, aktywna->y, dlugosc);
                Beep(750, 300);
            }
        }
    }
}

short idzDol(struct Litera * aktywna, char poleGry[maxPionowo][maxPoziomo], int * wynik, char ** slownik, long iloscSlow)
{
    short odp = 0;

    poleGry[aktywna->y][aktywna->x] = ' ';
    aktywna->y++;
    poleGry[aktywna->y][aktywna->x] = aktywna->znak;

    if(aktywna->y + 1 == maxPionowo || poleGry[aktywna->y + 1][aktywna->x] != ' ') odp = 1;
    if(odp == 1) szukajSlowa(aktywna, poleGry, wynik, slownik, iloscSlow);

    return odp;
}

short idzLewo(struct Litera * aktywna, char poleGry[maxPionowo][maxPoziomo], int * wynik, char ** slownik, long iloscSlow)
{
    poleGry[aktywna->y][aktywna->x] = ' ';
    if(aktywna->x > 0 && poleGry[aktywna->y][aktywna->x - 1] == ' ') aktywna->x--;
    poleGry[aktywna->y][aktywna->x] = aktywna->znak;
    return 0;
}

short idzPrawo(struct Litera * aktywna, char poleGry[maxPionowo][maxPoziomo], int * wynik, char ** slownik, long iloscSlow)
{
    poleGry[aktywna->y][aktywna->x] = ' ';
    if(aktywna->x < maxPoziomo-1 && poleGry[aktywna->y][aktywna->x + 1] == ' ') aktywna->x++;
    poleGry[aktywna->y][aktywna->x] = aktywna->znak;
    return 0;
}

short idzSzybko(struct Litera * aktywna, char poleGry[maxPionowo][maxPoziomo], int * wynik, char ** slownik, long iloscSlow)
{
    while(!idzDol(aktywna, poleGry, wynik, slownik, iloscSlow));
    return 1;
}

short wykonajRuch(struct Litera * aktywna, char poleGry[maxPionowo][maxPoziomo], int klawisz, int * wynik, char ** slownik, long iloscSlow)
{
    switch(klawisz)
    {
    case LEWY:
        if(aktywna->y == 0 && poleGry[1][aktywna->x] != ' ') return 0;
        return idzLewo(aktywna, poleGry, wynik, slownik, iloscSlow);
    case PRAWY:
        if(aktywna->y == 0 && poleGry[1][aktywna->x] != ' ') return 0;
        return idzPrawo(aktywna, poleGry, wynik, slownik, iloscSlow);
    case SZYBKI:
        if(aktywna->y == 0 && poleGry[1][aktywna->x] != ' ') return 0;
        return idzSzybko(aktywna, poleGry, wynik, slownik, iloscSlow);
    }
    return 0;
}

int main()
{
    srand(time(NULL));

    //Wczytywanie slownika
    FILE * plikSlownika;
    char ** slownik = (char**)malloc(100 * sizeof(char*));
    char slowo[maxSlowo+1];
    long iloscSlow = 0;

    plikSlownika = fopen("slownik.txt", "r");
        while(fscanf(plikSlownika, "%s\n", slowo) != EOF)
        {
            short dlugoscSlowa = strlen(slowo);

            *(slownik + iloscSlow) = (char*)malloc((dlugoscSlowa + 1) * sizeof(char));
            strcpy(*(slownik + iloscSlow), slowo);

            iloscSlow++;
            if(iloscSlow % 100 == 0) slownik = (char**)realloc(slownik, (iloscSlow + 100) * sizeof(char*));
        }
    fclose(plikSlownika);


    int menu;

    do
    {
        system("cls");
        printf(" ooooooooooooo oooooooooooo ooooooooooooo ooooooooo.   ooooo  .oooooo..o  \n"); 
        printf(" 8'   888   `8 `888'     `8 8'   888   `8 `888   `Y88. `888' d8P'    `Y8  \n");
        printf("      888       888              888       888   .d88'  888  Y88bo.       \n"); 
        printf("      888       888oooo8         888       888ooo88P'   888   `\"Y8888o.  \n");
        printf("      888       888    \"         888       888`88b.     888       `\"Y88b\n");
        printf("      888       888       o      888       888  `88b.   888  oo     .d8P  \n");
        printf("     o888o     o888ooooood8     o888o     o888o  o888o o888o 8""88888P' \n\n");
        printf("                                  +-+-+-+-+    \n");
        printf("                                  |M|E|N|U|    \n");
        printf("                                  +-+-+-+-+  \n\n");
        printf("                               [1] - Nowa Gra  \n");
		printf("                               [ESC] - Wyjscie \n");
        menu = getch();

        if(menu == 49) // Nowa Gra
        {
            // Inicjowanie pola gry
            char poleGry[maxPionowo][maxPoziomo];
            zerowaniePolaGry(poleGry);

            // Przebieg gry
            float ostatniCzas = clock() / 1000.0;
            struct Litera aktywna;
            short graTrwa = 1;
            short nowyZnak = 1;
            int klawisz;
            int wynik = 0;

            do
            {
                klawisz = -1;

                if(!graTrwa) break;

                if(nowyZnak)
                {
                    aktywna.x = rand() % maxPoziomo;
                    aktywna.y = 0;
                    aktywna.znak = (char)(rand() % 26 + 97);
                    poleGry[aktywna.y][aktywna.x] = aktywna.znak;
                    nowyZnak = 0;
                    pokazOknoGry(poleGry, wynik);
                }

                int i;
                for(i=0; i<maxPoziomo; i++)
                {
                    if(poleGry[0][i] != ' ' && poleGry[1][i] != ' ')
                    {
                        graTrwa = 0;
                        break;
                    }
                }

                if(graTrwa)
                {
                    if(kbhit())
                    {
                        klawisz = (int)tolower((char)getch());
                        if(klawisz == LEWY || klawisz == PRAWY || klawisz == SZYBKI)
                        {
                            nowyZnak = wykonajRuch(&aktywna, poleGry, klawisz, &wynik, slownik, iloscSlow);
                            pokazOknoGry(poleGry, wynik);
                        }
                    }

                    if(clock() / 1000.0 - ostatniCzas > czasNaRuch)
                    {
                        if(!nowyZnak) nowyZnak = idzDol(&aktywna, poleGry, &wynik, slownik, iloscSlow);
                        pokazOknoGry(poleGry, wynik);
                        ostatniCzas = clock() / 1000.0;
                    }
                }
            } while(klawisz != 27);

            system("cls");
            printf("Koniec gry! Twoj wynik to: %d!\n", wynik);
            system("pause");
        }
    } while(menu != 27);

    // Zwalnianie pamieci
    int i;
    for(i=0; i<iloscSlow; i++) free(*(slownik + i));
    free(slownik);

    return 0;
}

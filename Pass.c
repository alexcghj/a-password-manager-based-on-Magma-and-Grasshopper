#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "stribog.h"
#define DEFAULT_HASH_SIZE 512
#define FILE_BUFFER_SIZE 4096

StribogContext *Context;

struct login {
    char username[20];
    char password[20];
};

struct service {
    char service[20];
    char logins[20];
    char encrypt_pass[20];
};

void showMainMenu();
void showLoginMenu();
void showAuthMenu();
void showBaseMenu();
void showCreateMenu(short);
void showSwitchCreateMenu();
void showViewMenu();
void showHelpMenu();
void hash_stribog(char*, int);
void stribog_file( char*, int );

int checkPass(char *username, char *password);

int main() {

    initscr(); // Инициализация ncurses
    keypad(stdscr, TRUE); // Разрешение работы с клавиатурой
    noecho(); // Отключение отображения вводимых символов
    curs_set(FALSE); // Скрытие курсора

    showMainMenu();

    endwin(); // Завершение работы с ncurses
    return 0;
}

void showMainMenu() {
    int choice = 0;
    int highlight = 1;
    int ch;

    while (1) {
        clear(); // Очистка экрана
        printw("Welcome to your password manager!\n");

        // Создание бокса для меню
        WINDOW *menuWin = newwin(6, 20, 2, 2);
        //WINDOW *menuWin = newwin(menu_height, menu_width, menu_starty, menu_startx);

        box(menuWin, 0, 0);
        refresh();
        wrefresh(menuWin);

        // Отображение пунктов меню
        char *choices[] = {
            "Sign up",
            "Log in",
            "Help",
            "Exit"
        };

        for (int i = 0; i < 4; i++) {
            if (highlight == i + 1) {
                wattron(menuWin, A_REVERSE); // Включение реверсивного режима
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
                wattroff(menuWin, A_REVERSE); // Выключение реверсивного режима
            }
            else {
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
            }
        }
        mvprintw(9 , 0, "Created by: Siotanov Alexey & Kuleshov Alexander");
        wrefresh(menuWin);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                if (highlight > 1)
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 4)
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
        }

        if (choice != 0)
            break;
    }

    switch (choice) {
        case 1:
            showAuthMenu();
            break;
        case 2:
            showLoginMenu();
            break;
        case 3:
            //showHelpMenu();
            break;
        case 4:
            endwin();
            exit(1);
            break;
    }
}

void showLoginMenu() {
    clear();
    refresh();

    struct login loginstr;

    printw("\nLogin Menu\n");
    printw("Enter your username: ");
    refresh();
    getstr(loginstr.username);

    // Отображение символов ввода
    int x, y;
    getyx(stdscr, y, x); // Получаем текущие координаты курсора
    int ch;
    int i = 0;
    while ((ch = getch()) != '\n') {
        mvprintw(y, x + i, "%c", ch);
        loginstr.username[i++] = ch;
    }
    loginstr.username[i] = '\0';



    printw("\nEnter your password: ");
    refresh();
    noecho();
    getstr(loginstr.password);
    //TODO Здесь вывод символов
    /*
    int x1, y1;
    getyx(stdscr, y1, x1); // Получаем текущие координаты курсора

    // Отображение символов ввода
    int ch1;
    int j = 0;
    while ((ch1 = getch()) != '\n') {
        mvprintw(y1, x1 + j, "*");
        loginstr.password[j++] = ch1;
    }
    loginstr.password[j] = '\0';
    */
    
    Context = (StribogContext*)(malloc(sizeof(StribogContext)));

    int hash_size = DEFAULT_HASH_SIZE;
    
    hash_stribog(loginstr.password, hash_size);
    
    // Запись логина и пароля в файл
    FILE *file = fopen("logandpass.txt", "a"); // Открытие файла на дозапись
    if (file == NULL) {
        printw("Error opening file.\n");
        return;
    }

    //Вывод в файл
    fprintf(file, "%s:", loginstr.username);
    
    for(i = 0; i < 64; i++)
            fprintf(file, "%02x", Context->hash[i]);
    fprintf(file, "\n");

    fclose(file); // Закрытие файла

    printw("\nOperation completed! Press key to return.");
    refresh();
    getch(); // Ожидание нажатия клавиши
    showMainMenu();
}
void hash_stribog(char *str, int hash_size)
{
    uint8_t *buffer;
    buffer = malloc(strlen(str));
    memcpy(buffer, str, strlen(str));
    stribog_init(Context, hash_size);
    stribog_update(Context, buffer, strlen(str));
    stribog_final(Context);
}
void stribog_file( char *file_name, int hash_size)
{
    FILE *file;
    uint8_t *buffer;
    size_t len;
    stribog_init(Context, hash_size);
    if ((file = fopen(file_name, "rb")) != NULL)
    {
        buffer = malloc((size_t) FILE_BUFFER_SIZE);
        while ((len = fread(buffer, (size_t) 1, (size_t) FILE_BUFFER_SIZE, file))) stribog_update(Context, buffer, len);
        free(buffer);
        stribog_final(Context);
        fclose(file);
    }
    else
        printf("File error: %s\n", file_name);
}

void showAuthMenu() {
    clear();
    refresh();
    struct login loginstr;

    printw("Enter your username: ");
    refresh();
    getnstr(loginstr.username, 20);
    // Отображение символов ввода
    
    int x, y;
    getyx(stdscr, y, x); // Получаем текущие координаты курсора
    int ch;
    int i = 0;
    while ((ch = getch()) != '\n') {
        mvprintw(y, x + i, "%c", ch);
        loginstr.username[i++] = ch;
    }
    loginstr.username[i] = '\0';

    printw("\nEnter your password: ");
    refresh();
    noecho();
    getnstr(loginstr.password, 20);
    // Отображение символов ввода
    /*
    int ch1;
    int j = 0;
    int x1, y1;
    getyx(stdscr, y1, x1); // Получаем текущие координаты курсора
    while ((ch1 = getch()) != '\n') {
        mvprintw(y1, x1 + j, "*");
        loginstr.password[j++] = ch1;
    }
    loginstr.password[j] = '\0';
    */
    int result = checkPass(loginstr.username, loginstr.password);

    if (result == 1) {
        showBaseMenu();
    } else {
        printw("\nLogin failed. Incorrect username or password.\n");
    }

    refresh();
    getch(); // Ожидание нажатия клавиши
    showMainMenu();
}

int checkPass(char *username, char *password) {
    FILE *file = fopen("logandpass.txt", "r"); // Открытие файла на чтение
    if (file == NULL) {
        printw("Error opening file.\n");
        return -1;
    }

    char line[200]; // Предполагаем, что строка логина и пароля не превышает 200 символов

    while (fgets(line, sizeof(line), file)) {
        char *stored_username = strtok(line, ":"); // Имя пользователя, хранящееся в файле
        char *stored_password = strtok(NULL, ":"); // Пароль, хранящийся в файле

        stored_password[strlen(stored_password) - 1] = '\0'; // Удаление символа новой строки в конце пароля

        // Сравниваем введенные логин и пароль с хранящимися в файле
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
            fclose(file);
            return 1; // Логин и пароль совпадают
        }
    }

    fclose(file);
    return 0; // Логин и пароль не совпадают
}
void showBaseMenu() {
    int choice = 0;
    int highlight = 1;
    int ch;

    while (1) {
        clear(); // Очистка экрана
        printw("This is password Mannager\n");

        // Создание бокса для меню
        WINDOW *menuWin = newwin(10, 20, 2, 2);
        box(menuWin, 0, 0);
        refresh();
        wrefresh(menuWin);

        // Отображение пунктов меню
        char *choices[] = {
            "View",
            "Create",
            "Edit",
            "Delete",
            "Check summ",
            "Import/Export",
            "Help",
            "Exit"
        };

        for (int i = 0; i < 8; i++) {
            if (highlight == i + 1) {
                wattron(menuWin, A_REVERSE); // Включение реверсивного режима
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
                wattroff(menuWin, A_REVERSE); // Выключение реверсивного режима
            }
            else {
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
            }
        }

        wrefresh(menuWin);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                if (highlight > 1)
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 8)
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
        }

        if (choice != 0)
            break;
    }

    switch (choice) {
        case 1:
            showViewMenu();
            break;
        case 2:
            showSwitchCreateMenu();
            break;
        case 3:
            printw("Exiting program\n");
            break;
        case 4:
            printw("Exiting program\n");
            break;
        case 5:
            showMainMenu();
            break;
        case 8:
            endwin();
            showMainMenu();
            break;
    }
}
void showCheckSumMenu() {
    int choice = 0;
    int highlight = 1;
    int ch;

    while (1) {
        clear(); // Очистка экрана
        printw("This is password Mannager\n");

        // Создание бокса для меню
        WINDOW *menuWin = newwin(10, 20, 2, 2);
        box(menuWin, 0, 0);
        refresh();
        wrefresh(menuWin);

        // Отображение пунктов меню
        char *choices[] = {
            "Check file",
            "Exit"
        };

        for (int i = 0; i < 8; i++) {
            if (highlight == i + 1) {
                wattron(menuWin, A_REVERSE); // Включение реверсивного режима
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
                wattroff(menuWin, A_REVERSE); // Выключение реверсивного режима
            }
            else {
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
            }
        }

        wrefresh(menuWin);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                if (highlight > 1)
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 8)
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
        }

        if (choice != 0)
            break;
    }

    switch (choice) {
        case 1:
            //TODO Функция проверки контрольных сумм
            break;
        case 2:
            endwin();
            showBaseMenu();
            break;
    }
}
/*void checksum ()
{
	char buffer[512];
	stribog_file("secret.txt", hash_size);
	
}*/
void showViewMenu() {
    /*clear();
    refresh();
    struct login loginstr;

    char service[20];
    printw("Enter Service: ");
    refresh();
    getnstr(service, 20);

    int result = checkPass(loginstr.username, loginstr.password);

    if (result == 1) {
        showBaseMenu();
    } else {
        printw("\nLogin failed. Incorrect username or password.\n");
    }

    refresh();
    getch(); // Ожидание нажатия клавиши
    showMainMenu();*/
}
void showSwitchCreateMenu() {
    int choice = 0;
    int highlight = 1;
    int ch;
    int vibor = 0;

    while (1) {
        clear(); // Очистка экрана
        printw("Choose an algorithm\n");

        // Создание бокса для меню
        WINDOW *menuWin = newwin(5, 20, 2, 2);
        box(menuWin, 0, 0);
        refresh();
        wrefresh(menuWin);

        // Отображение пунктов меню
        char *choices[] = {
            "Kuznechik",
            "Magma",
            "Exit"
        };

        for (int i = 0; i < 3; i++) {
            if (highlight == i + 1) {
                wattron(menuWin, A_REVERSE); // Включение реверсивного режима
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
                wattroff(menuWin, A_REVERSE); // Выключение реверсивного режима
            }
            else {
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
            }
        }

        wrefresh(menuWin);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                if (highlight > 1)
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 3)
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
        }

        if (choice != 0)
            break;
    }

    switch (choice) {
        case 1:
            vibor = 1;
            showCreateMenu(vibor);
            break;
        case 2:
            vibor = 2;
            showCreateMenu(vibor);
            break;
        case 3:
            endwin();
            showBaseMenu();
            break;
    }
}
void showCreateMenu(short vibor) {
    clear();
    refresh();

    //struct login loginstr;
    struct service servicefile;
    char passwd[20];
    printw("\nCreate Password\n");

    printw("Enter service: ");
    refresh();
    getstr(servicefile.service);

    // Отображение символов ввода
    int x, y;
    getyx(stdscr, y, x); // Получаем текущие координаты курсора
    int ch;
    int i = 0;
    while ((ch = getch()) != '\n') {
        mvprintw(y, x + i, "%c", ch);
        servicefile.service[i++] = ch;
    }
    servicefile.service[i] = '\0';



    printw("\nEnter your login: ");
    refresh();
    noecho();
    getstr(servicefile.logins);
    int x1, y1;
    getyx(stdscr, y1, x1); // Получаем текущие координаты курсора

    // Отображение символов ввода
    int ch1;
    int j = 0;
    while ((ch1 = getch()) != '\n') {
        mvprintw(y1, x1 + j, "%c", ch1);
        servicefile.logins[j++] = ch1;
    }
    servicefile.logins[j] = '\0';

    printw("\nEnter your password: ");
    refresh();
    noecho();
    getstr(passwd);
    //getstr(servicefile.login);
    int x2, y2;
    getyx(stdscr, y2, x2); // Получаем текущие координаты курсора

    // Отображение символов ввода
    int ch2;
    int q = 0;
    while ((ch2 = getch()) != '\n') {
        mvprintw(y2, x2 + q, ch2);
        passwd[q++] = ch2;
    }
    passwd[q] = '\0';
    /*
     * СДЕЛАТЬ УСЛОВИЕ НА ОСНОВЕ Vibor
     * ВАЖНО!! ВЫЗВАТЬ ТУТА ФУНКЦИЮ ШИФРОВАНИЯ
     *
     *
     * */
    // Запись логина и пароля в файл
    FILE *file = fopen("secret.txt", "a"); // Открытие файла на дозапись
    if (file == NULL) {
        printw("Error opening file.\n");
        return;
    }

    fprintf(file, "%s:%s:%s\n", servicefile.service, servicefile.logins, servicefile.encrypt_pass);

    fclose(file); // Закрытие файла

    printw("\nOperation completed! Press key to return.");
    refresh();
    getch(); // Ожидание нажатия клавиши
    showBaseMenu(); 
}

/*void showHelpMenu() {
    int choice = 0;
    int highlight = 1;
    int ch;

    while (1) {
        clear(); // Очистка экрана
        printw("Welcome to your password manager!\n");
      
        // Создание бокса для меню
        WINDOW *menuWin = newwin(6, 20, 12, 2);
        WINDOW *helpWin = newwin(10,40,2,2);
        
        box(menuWin, 0, 0);
        box(newwin, 0, 0);
        refresh();
        wrefresh(menuWin);
        wrefresh(newwin);

        // Отображение пунктов меню
        char *choices[] = {
            "Exit"
        };

        for (int i = 0; i < 4; i++) {
            if (highlight == i + 1) {
                wattron(menuWin, A_REVERSE); // Включение реверсивного режима
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
                wattroff(menuWin, A_REVERSE); // Выключение реверсивного режима
            }
            else {
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
            }
        }
        //mvprintw(9 , 0, "Created by: Siotanov Alexey & Kuleshov Alexander");
        wrefresh(newwin);
        ch = getch();

        switch (ch) {
            case KEY_UP:
                if (highlight > 1)
                    highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 4)
                    highlight++;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
        }

        if (choice != 0)
            break;
    }

    switch (choice) {
        case 1:
            endwin();
            exit(1);
            break;
    }
}*/

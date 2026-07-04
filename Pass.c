#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include "vault.h"

#define MAXFIELD 256

static int menu_select(const char *title, const char *options[], int n) {
    int highlight = 0, ch;
    for (;;) {
        clear();
        mvprintw(0, 2, "%s", title);
        for (int i = 0; i < n; i++) {
            if (i == highlight) attron(A_REVERSE);
            mvprintw(2 + i, 4, "%s", options[i]);
            if (i == highlight) attroff(A_REVERSE);
        }
        mvprintw(3 + n, 2, "Arrow keys to move, Enter to select.");
        refresh();
        ch = getch();
        if (ch == KEY_UP)        highlight = (highlight - 1 + n) % n;
        else if (ch == KEY_DOWN) highlight = (highlight + 1) % n;
        else if (ch == '\n' || ch == KEY_ENTER) return highlight;
    }
}

static void read_line(int row, const char *prompt, char *buf, int maxlen, int mask) {
    mvprintw(row, 2, "%s", prompt); clrtoeol(); refresh();
    int i = 0, ch, col = 2 + (int)strlen(prompt);
    curs_set(TRUE);
    for (;;) {
        ch = getch();
        if (ch == '\n' || ch == KEY_ENTER) break;
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && i > 0) {
            i--; mvaddch(row, col + i, ' '); move(row, col + i); refresh(); continue;
        }
        if (ch >= 32 && ch < 127 && i < maxlen - 1) {
            buf[i++] = (char)ch; addch(mask ? '*' : ch); refresh();
        }
    }
    buf[i] = '\0'; curs_set(FALSE);
}

static void msg(int row, const char *m) {
    mvprintw(row, 2, "%s", m);
    mvprintw(row + 1, 2, "Press any key to continue...");
    refresh(); getch();
}

static void view_cb(const char *svc, const char *login, const char *pw, void *ctx) {
    int *row = (int *)ctx;
    mvprintw((*row)++, 4, "%-16s  %-20s  %s", svc, login, pw);
}

static void vault_menu(const char *username) {
    const char *opts[] = { "Add entry", "View entries", "Log out" };
    for (;;) {
        char title[320];
        snprintf(title, sizeof(title), "Vault  -  logged in as %s", username);
        int c = menu_select(title, opts, 3);
        if (c == 0) {
            char service[MAXFIELD], login[MAXFIELD], password[MAXFIELD];
            clear(); mvprintw(0, 2, "Add a new entry");
            read_line(2, "Service:  ", service,  MAXFIELD, 0);
            read_line(3, "Login:    ", login,    MAXFIELD, 0);
            read_line(4, "Password: ", password, MAXFIELD, 1);
            add_entry(username, service, login, password);
            msg(6, "Saved (encrypted with Magma).");
        } else if (c == 1) {
            clear(); mvprintw(0, 2, "Your entries:");
            mvprintw(1, 4, "%-16s  %-20s  %s", "SERVICE", "LOGIN", "PASSWORD");
            int row = 2;
            list_entries(username, view_cb, &row);
            if (row == 2) mvprintw(2, 4, "(no entries yet)");
            msg(row + 1, "");
        } else {
            return;
        }
    }
}

int main(void) {
    initscr(); keypad(stdscr, TRUE); noecho(); curs_set(FALSE);
    const char *opts[] = { "Register", "Log in", "Exit" };
    for (;;) {
        int c = menu_select("GOST Password Manager (Magma + Streebog)", opts, 3);
        if (c == 0) {
            char user[MAXFIELD], pass[MAXFIELD];
            clear(); mvprintw(0, 2, "Register a new account");
            read_line(2, "Username: ", user, MAXFIELD, 0);
            read_line(3, "Password: ", pass, MAXFIELD, 1);
            int r = register_user(user, pass);
            msg(5, r == 0 ? "Account created." : "That username already exists.");
        } else if (c == 1) {
            char user[MAXFIELD], pass[MAXFIELD];
            clear(); mvprintw(0, 2, "Log in");
            read_line(2, "Username: ", user, MAXFIELD, 0);
            read_line(3, "Password: ", pass, MAXFIELD, 1);
            if (verify_login(user, pass)) { start_session(pass); vault_menu(user); }
            else msg(5, "Login failed.");
        } else break;
    }
    endwin();
    return 0;
}

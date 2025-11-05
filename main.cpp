#include <iostream>
#include <ncurses.h>
#include <vector>
#include <unistd.h>
#include <ctime>

struct SnakeType
{
    int s_x, s_y;

    SnakeType(int s_col, int s_row)
    {
        s_x = s_col;
        s_y = s_row;
    }

    SnakeType()
    {
        s_x = 0;
        s_y = 0;
    }
};

class SnakeGame
{
protected:
    int m_maxwidth;
    int m_maxheight;

    std::vector<SnakeType> snake;
    char m_snake_char;

    SnakeType v_food;
    char m_food_char;

    int m_delay;
    char m_direction;
    bool m_tail_stop;

public:
    SnakeGame()
    {
        initscr();
        nodelay(stdscr, true);
        keypad(stdscr, true);
        noecho();
        curs_set(0);

        m_maxwidth = getmaxx(stdscr);
        m_maxheight = getmaxy(stdscr);

        m_snake_char = 'O';
        m_food_char = 'X';
        srand(time(NULL));

        m_delay = 100000;
        m_direction = 'L';
        m_tail_stop = false;

        // Desenha bordas
        draw_borders();

        // Snake inicial
        for (int i = 0; i < 5; ++i)
        {
            snake.push_back(SnakeType(m_maxwidth / 2 + i, m_maxheight / 2));
        }

        // Inserir comida
        m_insert_food();

        // Desenha snake e comida
        for (auto &seg : snake)
        {
            move(seg.s_y, seg.s_x);
            addch(m_snake_char);
        }

        move(v_food.s_y, v_food.s_x);
        addch(m_food_char);
        refresh();
    }

    ~SnakeGame()
    {
        endwin();
    }

    void draw_borders()
    {
        // Topo e fundo
        for (int i = 0; i < m_maxwidth; ++i)
        {
            move(0, i);
            addch((i == 0 || i == m_maxwidth - 1) ? '+' : '-');
            move(m_maxheight - 1, i);
            addch((i == 0 || i == m_maxwidth - 1) ? '+' : '-');
        }

        // Laterais
        for (int i = 1; i < m_maxheight - 1; ++i)
        {
            move(i, 0);
            addch('|');
            move(i, m_maxwidth - 1);
            addch('|');
        }
    }

    void m_insert_food()
    {
        while (true)
        {
            int tmpx = rand() % (m_maxwidth - 2) + 1;
            int tmpy = rand() % (m_maxheight - 2) + 1;

            bool on_snake = false;
            for (auto &seg : snake)
            {
                if (seg.s_x == tmpx && seg.s_y == tmpy)
                {
                    on_snake = true;
                    break;
                }
            }

            if (on_snake)
                continue;

            v_food.s_x = tmpx;
            v_food.s_y = tmpy;

            move(v_food.s_y, v_food.s_x);
            addch(m_food_char);
            refresh();
            break;
        }
    }

    void moveasnake()
    {
        int tmp = getch();
        switch (tmp)
        {
        case KEY_LEFT:
            if (m_direction != 'R')
                m_direction = 'L';
            break;
        case KEY_UP:
            if (m_direction != 'D')
                m_direction = 'U';
            break;
        case KEY_DOWN:
            if (m_direction != 'U')
                m_direction = 'D';
            break;
        case KEY_RIGHT:
            if (m_direction != 'L')
                m_direction = 'R';
            break;
        case 'q':
        case 'Q':
            endwin();
            exit(0);
        }

        if (!m_tail_stop)
        {
            move(snake.back().s_y, snake.back().s_x);
            printw(" ");
            refresh();
            snake.pop_back();
        }

        if (m_direction == 'L')
            snake.insert(snake.begin(), SnakeType(snake[0].s_x - 1, snake[0].s_y));
        if (m_direction == 'R')
            snake.insert(snake.begin(), SnakeType(snake[0].s_x + 1, snake[0].s_y));
        if (m_direction == 'U')
            snake.insert(snake.begin(), SnakeType(snake[0].s_x, snake[0].s_y - 1));
        if (m_direction == 'D')
            snake.insert(snake.begin(), SnakeType(snake[0].s_x, snake[0].s_y + 1));

        move(snake[0].s_y, snake[0].s_x);
        addch(m_snake_char);
    }

    void start()
    {
        while (true)
        {
            moveasnake();

            // Comer comida
            if (snake[0].s_x == v_food.s_x && snake[0].s_y == v_food.s_y)
            {
                m_tail_stop = true;
                m_insert_food();
            }
            else
            {
                m_tail_stop = false;
            }

            // Colisão com parede
            if (snake[0].s_x <= 0 || snake[0].s_x >= m_maxwidth - 1 ||
                snake[0].s_y <= 0 || snake[0].s_y >= m_maxheight - 1)
            {
                break;
            }

            // Colisão com corpo
            for (size_t i = 1; i < snake.size(); i++)
            {
                if (snake[0].s_x == snake[i].s_x && snake[0].s_y == snake[i].s_y)
                {
                    endwin();
                    std::cout << "Game Over! Você colidiu com você mesmo.\n";
                    return;
                }
            }

            refresh();
            usleep(m_delay);
        }

        endwin();
        std::cout << "Game Over! Você bateu na parede.\n";
    }
};

int main()
{
    SnakeGame s;
    s.start();
    return 0;
}

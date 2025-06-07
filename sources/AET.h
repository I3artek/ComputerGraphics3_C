#ifndef MY_RAYLIB_GAME_AET_H
#define MY_RAYLIB_GAME_AET_H

struct edge {
    int y_max;
    int x_min;
    double x;
    double inv_m;
    struct edge *next;
    edge(int x0, int y0, int x1, int y1) {
        if(y0 > y1) {
            y_max = y0;
            x_min = x1;
        } else {
            y_max = y1;
            x_min = x0;
        }
        inv_m = (double)(x1 - x0) / (double)(y1 - y0);
        x = (double)x_min;
    }
    ~edge() {
    }
    void update_x() {
        //printf("%d + %f = %d\n", x_min, inv_m, (int)((double)x_min + inv_m));
        x += inv_m;
        x_min = (int)(x);
    }
    bool operator< (const edge& e) const {
        return x_min < e.x_min;
    }
};

typedef struct edge edge;

void insert(edge **head, edge *e) {
    if(*head == nullptr) {
        *head = e;
    } else {
        e->next = *head;
        *head = e;
    }
}

void sort_by_x(edge **head) {
    // sort the linked list by ascending x value
}

void insert_to_vector(edge **head, std::vector<edge *> &vec) {
    edge *tmp = *head;
    while (tmp != nullptr) {
        vec.push_back(tmp);
        //printf("[%s:%d] tmp: %p\n", __func__, __LINE__, tmp);
        //printf("[%s:%d] vec size: %d\n", __func__, __LINE__, vec.size());
        tmp = tmp->next;
    }
    *head = nullptr;
}

#endif //MY_RAYLIB_GAME_AET_H

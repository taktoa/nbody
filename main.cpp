#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <stdlib.h>
#include <time.h>

using namespace std;

typedef struct {
    double x, y;
} point_t;

static inline point_t point_make(double x, double y) {
    return (point_t) { x, y };
}

static inline double point_x(const point_t& point) { return point.x; }
static inline double point_y(const point_t& point) { return point.y; }

static void print_point(const point_t& point) {
    cout << "{ x = " << point.x
         << "; y = " << point.y
         << "; }";
}

typedef struct {
    double dx, dy;
} vec_t;

static inline vec_t vec_make(double dx, double dy) {
    return (vec_t) { dx, dy };
}

static inline double vec_x(const vec_t& vec) { return vec.dx; }
static inline double vec_y(const vec_t& vec) { return vec.dy; }

static void print_vec(const vec_t& vec) {
    cout << "{ dx = " << vec.dx
         << "; dy = " << vec.dy
         << "; }";
}

typedef struct {
    point_t pos;
    double  mass;
} body_t;

static inline body_t body_make(point_t pos, double mass) {
    return (body_t) { pos, mass };
}

static inline body_t body_make(double x, double y, double mass) {
    return (body_t) { { x, y }, mass };
}

static inline double     body_x(const body_t& body) { return body.pos.x; }
static inline double     body_y(const body_t& body) { return body.pos.y; }
static inline point_t  body_pos(const body_t& body) { return body.pos; }
static inline double  body_mass(const body_t& body) { return body.mass; }

static body_t make_random_body(double radius,
                               double min_mass,
                               double max_mass) {
    double mass = (rand() % ((int) (max_mass - min_mass))) + min_mass;
    double x = (rand() % ((int) (2 * radius))) - radius;
    double y = (rand() % ((int) (2 * radius))) - radius;
    return body_make(x, y, mass);
}

static void print_body(const body_t& body) {
    cout << "{"
         << " mass = " << body.mass << ";"
         << " pos = ";
    print_point(body.pos);
    cout << "; }";
}

typedef struct {
    list<body_t>* bodies;
} state_t;

static inline state_t state_make() {
    list<body_t>* bodies = new list<body_t>();
    return (state_t) { bodies };
}

static inline void state_add_body(state_t& state, const body_t& body) {
    state.bodies->push_back(body);
}

static state_t make_random_state(size_t number,
                                 double radius,
                                 double min_mass,
                                 double max_mass) {
    state_t state = state_make();
    for(size_t i = 0; i < number; ++i) {
        state_add_body(state, make_random_body(radius, min_mass, max_mass));
    }
    return state;
}

static void print_state(state_t& state) {
    cout << "{ state = ";
    cout << "[ ";
    for(const auto& body : *(state.bodies)) {
        print_body(body);
        cout << " ";
    }
    cout << "]; }";
}

static bool body_near_body(double        threshold,
                           const body_t& body1,
                           const body_t& body2) {
    const double dx = body1.pos.x - body2.pos.x;
    const double dy = body1.pos.y - body2.pos.y;
    const double dist = (dx + dy) / (body1.mass * body2.mass);
    return dist < threshold;
}

typedef struct {
    vector<body_t>* bodies;
} partition_t;

static inline partition_t partition_make() {
    vector<body_t>* bodies = new vector<body_t>();
    return (partition_t) { bodies };
}

static inline void partition_add(partition_t&  partition,
                                 const body_t& body) {
    partition.bodies->push_back(body);
}

static partition_t partition_state(double threshold, state_t& state) {
    list<body_t>* sbs = state.bodies;
    const int index = rand() % sbs->size();
    body_t* chosen_ptr = NULL;
    {
        int i = 0;
        for(auto it = sbs->begin(); it != sbs->end(); ++it) {
            if(i == index) { chosen_ptr = &*it; }
            ++i;
        }
    }
    const body_t chosen = *chosen_ptr;
    partition_t partition = partition_make();
    for(list<body_t>::iterator it = sbs->begin(); it != sbs->end(); ++it) {
        body_t body = *it;
        if(body_near_body(threshold, chosen, body)) {
            partition_add(partition, body);
            it = sbs->erase(it);
        }
    }
    return partition;
}

static void print_partition(partition_t& partition) {
    cout << "{ partition = ";
    cout << "[ ";
    for(const auto& body : *(partition.bodies)) {
        print_body(body);
        cout << " ";
    }
    cout << "]; }";
}

int main(int argc, char** argv) {
    srand(time(NULL));
    int num_bodies = 60;
    double radius = 200;
    double min_mass = 1;
    double max_mass = 100;
    state_t state = make_random_state(num_bodies, radius, min_mass, max_mass);
    print_state(state);
    cout << endl;
    cout << endl;

    const double avg_mass = (min_mass + max_mass) / 2;
    const double threshold = radius / (5 * avg_mass * avg_mass);
    vector<partition_t> partitions = vector<partition_t>();
    while(state.bodies->size() > 0) {
        partition_t partition = partition_state(threshold, state);
        print_partition(partition);
        cout << endl;
        partitions.push_back(partition);
    }
    return 0;
}

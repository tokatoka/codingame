#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/



class Point{
    private:
        float x,y;
    public:
        Point() : x(0), y(0){}

        Point(float X, float Y) : x(X), y(Y){}
        float norm2(){
            return x * x + y * y;
        }
        float dot(Point p){
            return x*p.x + y*p.y;
        }
        float getX(){
            return x;
        }
        float getY(){
            return y;
        }
        float dist(Point p){
            return sqrt(pow((x - p.x), 2) + pow((y - p.y), 2));
        }
        Point operator+(const Point &rhs){
            return Point(x + rhs.x, y + rhs.y);
        }
        Point operator-(const Point &rhs){
            return Point(x - rhs.x, y - rhs.y);
        }
        Point operator*(float c){
            return Point(x * c, y * c);
        }
        bool operator==(const Point &rhs){
            return (rhs.x == x && rhs.y == y);
        }
        friend ostream& operator<<(ostream &out, const Point &c);
};

ostream & operator << (ostream &out, const Point &c) 
{ 
    out << "x : " << c.x << " y : " << c.y;
    return out;
}


class Manager{
    public:
        vector<Point> checkpoints;
        vector<float> distance;
        int maxdistidx = 0;
        void insertCheckpoint(Point cp){
            checkpoints.push_back(cp);
        }
        void updateDistance(){
            distance.push_back(checkpoints[0].dist(checkpoints[checkpoints.size() - 1]));
            for(int i = 1; i < checkpoints.size(); i++){
                distance.push_back(checkpoints[i].dist(checkpoints[i-1]));
            }
            float max_dist = -1.0;
            for(int i = 0; i < distance.size(); i++){
                if(distance[i] > max_dist){
                    max_dist = distance[i];
                    maxdistidx = i;
                }
            }
        }
};
class Pod{
    public:
        Point _xy;
        Point _vxy;
        float _angle = 0.0;
        int _nextCheckPointId;
        int _targetangle;
        Manager _mgr;
        Pod(Manager mgr) : _mgr(mgr){
        }
        void update(float x, float y, float vx, float vy,float angle, int nextCheckPointId){
            _xy = Point(x,y);
            _vxy = Point(vx,vy); 
            _angle = angle;
            _nextCheckPointId = nextCheckPointId;

            Point vec = _mgr.checkpoints[_nextCheckPointId] - _xy;
            _targetangle = atan2(vec.getY(), vec.getX()) * 180/ M_PI;
            _targetangle = (_targetangle + 360) % 360;
        }

};

class PlayerPod : public Pod{
    public:
        int _thrust = 0;
        bool hasBoost = true;
        int _anglediff = 0;
        Point _target;
        PlayerPod(Manager mgr) : Pod(mgr){}
        void decide(){
            _target = Point(_mgr.checkpoints[_nextCheckPointId]) - _vxy * 3;
            _anglediff = int(abs(_angle - _targetangle)) % 360;
            _anglediff = _anglediff > 180 ? 360 - _anglediff : _anglediff;
            if(_anglediff > 90){
                _thrust = 0;
            }
            else{
                _thrust = 100;
                float alpha = cos(M_PI * _anglediff / 180);
                _thrust = alpha * _thrust;
            }
        }
        void runnergo(){
            if(hasBoost  && _nextCheckPointId == _mgr.maxdistidx && _anglediff < 5 && _anglediff > -5){
                cout << _target.getX() << " " << _target.getY() << " " << "BOOST" << endl;  
                hasBoost = false; 
            }
            else{
                cout << _target.getX() << " " << _target.getY() << " " << _thrust << endl;
            }
        }
        void debug(){
            cerr << _anglediff << endl;
            cerr << _nextCheckPointId << endl;
            cerr << _xy << endl;
            cerr << _vxy << endl;
        }
};


Pod *e1;
Pod *e2;
PlayerPod *runner;
class Attacker : public PlayerPod{
    private:
        int useShield = 0;
    public:
        Attacker(Manager mgr) : PlayerPod(mgr){}
        void decideShield(){
            if(_xy.dist(e1->_xy) < _xy.dist(e2->_xy)){
                useShield = 1;
            }
            if(_xy.dist(e1->_xy) >= _xy.dist(e2->_xy)){
                useShield = 2;
            }
        }
        void attackergo(){
            decideShield();
            switch (useShield){
                case 0:
                    cout << _target.getX() << " " << _target.getY() << " " << _thrust << endl;
                    break;
                case 1:
                    cout << e1->_xy.getX() << " " << e1->_xy.getY() << " SHIELD" << endl;
                    break;
                case 2:
                    cout << e2->_xy.getX() << " " << e2->_xy.getY() << " SHIELD" << endl;
                    break;
            }
            useShield = 0;
        }
};
Attacker *attacker;

int main()
{
    vector<Point> checkpoints;
    int laps;
    cin >> laps;
    int checkpointCount;
    cin >> checkpointCount;
    Manager mgr;
    for(int i = 0; i < checkpointCount; i++){
        int checkpointX, checkpointY;
        cin >> checkpointX >> checkpointY;
        mgr.insertCheckpoint(Point(checkpointX,checkpointY));
    }
    mgr.updateDistance();

    e1 = new Pod(mgr);
    e2 = new Pod(mgr);
    runner = new PlayerPod(mgr);
    attacker = new Attacker(mgr);
    // game loop
    while (1) {
        for(int i = 0; i < 4; i++){
            float x;
            float y;
            float vx;
            float vy;
            float angle;
            int nextCheckPointId;
            cin >> x >> y >> vx >> vy >> angle >> nextCheckPointId;
            switch (i) {
                case 0:
                    runner->update(x, y, vx, vy, angle, nextCheckPointId);
                    break;
                case 1:
                    attacker->update(x, y, vx, vy, angle, nextCheckPointId);
                    break;
                case 2:
                    e1->update(x, y, vx, vy, angle, nextCheckPointId);
                    break;
                case 3:
                    e2->update(x, y, vx, vy, angle, nextCheckPointId);
                    break;
            }
        }
        runner->decide();
        attacker->decide();
        //runner->debug();
        //attacker->debug();

        runner->runnergo();
        attacker->runnergo();
    }
}

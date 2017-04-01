#ifndef QuadTree_h_
#define QuadTree_h_

#include <memory>
#include <vector>

namespace bali 
{
    namespace qt
    {
        class XY//Point
        {
        public:
            float x;
            float y;
            uint32_t tx;
            uint32_t ty;
            uint32_t ti;
        };

        class AABB//Axis-Aligned Bounding Box
        {
        public:
            AABB() {}
            AABB(float minx, float miny, float maxx, float maxy)
            {
                min.x = minx;
                min.y = miny;
                max.x = maxx;
                max.y = maxy;
            }

            bool containsPoint(const XY & point)
            {
                if (point.x >= min.x && point.x <= max.x &&
                    point.y >= min.y && point.y <= max.y)
                    return true;
                return false;
            }

            bool intersects(const AABB & a)
            {
                //http://silentmatt.com/rectangle-intersection/
                if (min.x < a.max.x &&
                    max.x > a.min.x &&
                    min.y < a.max.y &&
                    max.y > a.min.y) {
                    // We have an overlap
                    return true;
                }
                return false;
            }

            XY min;
            XY max;
        };

        class Node
        {
        public:

            Node(AABB b) {
                nw = ne = se = sw = nullptr;
                boundary = b;
                isleaf = 1;
                depth = 0;
            }

            // to represent the boundaries of this quad tree
            AABB boundary;

            // Points in this quad tree node
            std::vector<XY> points;
            int depth;

            Node * nw;
            Node * ne;
            Node * se;
            Node * sw;

            //flag to enable reuse
            int isleaf;
        };

        class QuadTree
        {
        public:
            typedef std::shared_ptr<QuadTree> Ptr;
            typedef std::vector<Ptr> Vec;
            typedef std::shared_ptr<std::vector<Ptr>> VecPtr;
            QuadTree()
            {
                theRoot = nullptr;
            }

            QuadTree(const AABB & b, int max_depth)
            {
                theRoot = nullptr;
                initialize(b, max_depth);
            }
            ~QuadTree() {
                cleanup();
            }

            void initialize(const AABB & b, int max_depth)
            {
                maxDepth = max_depth;
                topLevelBoundary = b;
                theRoot = newNode(topLevelBoundary, 0);
                theRoot->isleaf = 1;
            }

            bool cleanup()
            {
                return cleanup(&theRoot);
            }

            bool cleanup(Node **node)
            {
                if (node == nullptr)
                    return false;
                if ((*node) == nullptr)
                    return false;

                cleanup(&(*node)->nw);
                cleanup(&(*node)->ne);
                cleanup(&(*node)->se);
                cleanup(&(*node)->sw);

                delete *node;
                return true;
            }

            typedef void(*visitFunc) (Node *node);
            bool visitAll(visitFunc vf)
            {
                return visitAll(&theRoot, vf);
            }

            bool visitAll(Node **node, visitFunc vf)
            {
                if ((*node) == nullptr)
                    return true;// TODO ?

                vf((*node));

                visitAll(&(*node)->nw, vf);
                visitAll(&(*node)->ne, vf);
                visitAll(&(*node)->se, vf);
                visitAll(&(*node)->sw, vf);
                return true;
            }

            std::vector<XY> search(const AABB & searchRect)
            {
                std::vector<XY> points;
                search(theRoot, searchRect, points);
                return points;
            }

            bool search(Node *node, const AABB & searchRect, std::vector<XY> &points)
            {
                if (node == nullptr)
                    return false;

                if (node->boundary.intersects(searchRect))
                {
                    for (auto p = 0; p < node->points.size(); p++)
                    {
                        points.push_back(node->points[p]);
                    }

                    search(node->nw, searchRect, points);
                    search(node->ne, searchRect, points);
                    search(node->se, searchRect, points);
                    search(node->sw, searchRect, points);
                }
                return true;
            }

            bool insert(const XY & point) {
                //cout << "Inserting Point(" << point.x << ", " << point.y << ")" << endl;
                return insert(&theRoot, point);
            }
            bool insert(Node **node, const XY & point)
            {
                if (!(*node)->boundary.containsPoint(point))
                    return false;

                if ((*node)->isleaf)
                {
                    if ((*node)->points.size() < PointCap || (*node)->depth == maxDepth)// || (*node)->pointCapOverride==1)
                    {
                        //cout << "Point<(" << point.x << ", " << point.y <<")> Inserted at <(" <<(*node)->boundary.min.x << ", " << (*node)->boundary.min.y<< "),("<< (*node)->boundary.max.x<< ", " << (*node)->boundary.max.y<< ")>" << endl;
                        (*node)->points.push_back(point);
                        //cout << "point.size() = " << (*node)->points.size() << endl;
                        return true;
                    }
                }
                else {

                    //cout << "Slipping down past <(" <<(*node)->boundary.min.x << ", " << (*node)->boundary.min.y<< "),("<< (*node)->boundary.max.x<< ", " << (*node)->boundary.max.y<< ")>" << endl;
                }
                if ((*node)->nw == nullptr) {//  && (*node)->depth < 2){
                    (*node)->isleaf = 0;
                    subDivide(node);
                }


                if (insert(&((*node)->nw), point))
                    return true;
                if (insert(&((*node)->ne), point))
                    return true;
                if (insert(&((*node)->se), point))
                    return true;
                if (insert(&((*node)->sw), point))
                    return true;

                return false;

            }
            void subDivide(Node **node)
            {
                float minx, miny;
                float maxx, maxy;
                float hafwidth, hafheight;

                minx = (*node)->boundary.min.x;
                miny = (*node)->boundary.min.y;
                maxx = (*node)->boundary.max.x;
                maxy = (*node)->boundary.max.y;
                hafwidth = (maxx - minx) / 2.0f;
                hafheight = (maxy - miny) / 2.0f;

                AABB a(minx, miny, maxx - hafwidth, maxy - hafheight);//nw
                AABB b(minx + hafwidth, miny, maxx, maxy - hafheight);//ne
                AABB c(minx + hafwidth, miny + hafheight, maxx, maxy);//se
                AABB d(minx, miny + hafheight, maxx - hafwidth, maxy);//sw

                                                                      //        cout << "Subdividing <(" <<minx << ", " << miny<< "),("<< maxx << ", " << maxy<< ")>" << endl;
                                                                      //        cout << "New boundaries: NW<(" <<a.min.x << ", " << a.min.y<< "),("<< a.max.x<< ", " << a.max.y<< ")>" << endl;
                                                                      //        cout << "                NE<(" <<b.min.x << ", " << b.min.y<< "),("<< b.max.x<< ", " << b.max.y<< ")>" << endl;
                                                                      //        cout << "                SE<(" <<c.min.x << ", " << c.min.y<< "),("<< c.max.x<< ", " << c.max.y<< ")>" << endl;
                                                                      //        cout << "                SW<(" <<d.min.x << ", " << d.min.y<< "),("<< d.max.x<< ", " << d.max.y<< ")>" << endl << endl;


                (*node)->nw = newNode(a, (*node)->depth + 1);
                (*node)->ne = newNode(b, (*node)->depth + 1);
                (*node)->se = newNode(c, (*node)->depth + 1);
                (*node)->sw = newNode(d, (*node)->depth + 1);

                for (auto i = (*node)->points.begin(); i != (*node)->points.end(); i++)
                {

                    if (insert(&((*node)->nw), (*i))) {
                        //cout << "copied (" << (*i).x << ", " << (*i).y << ") down to nw" << endl;
                        continue;
                    }
                    if (insert(&((*node)->ne), (*i))) {
                        //cout << "copied (" << (*i).x << ", " << (*i).y << ") down to ne" << endl;
                        continue;
                    }
                    if (insert(&((*node)->se), (*i))) {
                        //cout << "copied (" << (*i).x << ", " << (*i).y << ") down to se" << endl;
                        continue;
                    }
                    if (insert(&((*node)->sw), (*i))) {
                        //cout << "copied (" << (*i).x << ", " << (*i).y << ") down to sw" << endl;
                        continue;
                    }
                }
                (*node)->points.clear();
            }

            Node * newNode(const AABB & b, int depth)
            {
                Node *node = new Node(b);
                node->depth = depth;
                return node;
            }
            AABB topLevelBoundary;
            Node *theRoot;
            int maxDepth;
            const int PointCap = 1;

        };
    }//end namespace qt

}//end namespace bali
#endif



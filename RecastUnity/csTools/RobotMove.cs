using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

public class RobotMove : MonoBehaviour {

    public Vector3 roadpoint = new Vector3(100,0,100);

    public Vector3 min = new Vector3(0, 0, 0);
    public Vector3 max = new Vector3(0, 0, 0);

    public bool atpoint = false;
    public Vector3 _endpos;
    public List<Vector3> pathlist = new List<Vector3>();
    public List<Vector3> PathMap = new List<Vector3>();
    public HashSet<Vector3> ErrorPoint = new HashSet<Vector3>();
    // Use this for initialization
    void Start () {
        restarcomput();
    }
    void restarcomput()
    {
        pathlist.Clear();
        Vector3 point = PathMap[Random.Range(0, PathMap.Count-1)];
        while (!RecastMove.is_valid_point(point.x, point.y, point.z))
        {
            point = PathMap[Random.Range(0, PathMap.Count - 1)];
            //point = new Vector3(Random.Range(min.x, max.x), 0.0f, Random.Range(min.z, max.z));
        }

        float[] start = new float[3];
        start[0] = this.transform.position.x;
        start[1] = this.transform.position.y;
        start[2] = this.transform.position.z;

        float[] end = new float[3];
        end[0] = point.x;
        end[1] = point.y;
        end[2] = point.z;


        int maxPoint = 256 * 3 * 3;
        float[] _path = new float[maxPoint];
        int patchcount = 0;

        RecastMove.findPath(start, end, _path, ref patchcount);
        for (int i = 0; i < patchcount * 3;)
        {
            Vector3 _i = new Vector3(_path[i], _path[i + 1], _path[i + 2]);
            pathlist.Add(_i);
            i += 3;
        }
        _endpos = this.transform.position;
    }
    public void AudoMove(Vector3 _StartPos , Vector3 _EndtPos)
    {
        {
            Vector3 v = (_EndtPos - _StartPos);
            Vector2 _EndtPos2 = new Vector2(_EndtPos.x, _EndtPos.z);
            Vector2 _Pos2 = new Vector2(gameObject.transform.position.x, gameObject.transform.position.z);
            Vector2 vt = _EndtPos2 - _Pos2;
            if (vt.magnitude < 5.315837E-05)
            {
                atpoint = true;
                _endpos = pathlist[0];
                pathlist.RemoveAt(0);
            }
           else
           {
               // Vector3 ff = Vector3.MoveTowards(_StartPos, _EndtPos, 1.0f);
               gameObject.transform.position = Vector3.MoveTowards(gameObject.transform.position, _EndtPos,0.1f);
               Vector3 _point = gameObject.transform.position;
                _point.y = 5000;
               RaycastHit hit;
                if (Physics.Raycast(new Ray(_point, new Vector3(0, -1, 0)), out hit, 200000.0f))
                {
                    gameObject.transform.position = new Vector3(gameObject.transform.position.x, hit.point.y, gameObject.transform.position.z);
                }
                else {
                    if (ErrorPoint.Add(gameObject.transform.position))
                    {
                        Debug.LogWarningFormat("寻路机器人走到这里掉下去了 X{0},  Y{1},  Z{2}", gameObject.transform.position.x,
                            gameObject.transform.position.y,
                            gameObject.transform.position.z);
#if UNITY_EDITOR
                        GameObject tempQie = AssetDatabase.LoadAssetAtPath("Assets/Models/obstacle_perfab/CubeOBBOX.prefab", typeof(GameObject)) as GameObject;
                        GameObject tempQie2 = (GameObject)GameObject.Instantiate(tempQie);
                        tempQie2.GetComponent<MeshRenderer>().enabled = true;
                        tempQie2.GetComponent<BoxCollider>().enabled = false;
                        //tempQie2.AddComponent<RobotMove>();
                        //tempQie2.GetComponent<RobotMove>().PathMap = PathMap;
                        //tempQie2.GetComponent<RobotMove>().max = new Vector3(1, 10, 1);
                        //tempQie2.GetComponent<RobotMove>().min = new Vector3(1, 10, 1);
                        tempQie2.transform.position = gameObject.transform.position;
                        tempQie2.transform.localScale = new Vector3(1, 10, 1);
#endif

                    }
                }
            }
        }
    }
	
	// Update is called once per frame
	void Update () {
        if (pathlist.Count >= 2)
            AudoMove(pathlist[0], pathlist[1]);
        else
        {
            restarcomput();
        }
    }
}

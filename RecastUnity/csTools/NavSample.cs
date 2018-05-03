using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
    using UnityEditor;
#endif
using System;
using System.Runtime.InteropServices;
public class NavSample : MonoBehaviour {

    private DbgRenderMesh Dbgrendermesh = new DbgRenderMesh();

    private Material Debugrendermeshmaterial = null;
    private Color _vcolor = new Color(109/255.0f,144 / 255.0f, 145 / 255.0f, 200 / 255.0f);
    public Vector3 ObstaclesSize;
    public float  MapAcc = 0.1f;

    public Bounds _Bounds;

    private List<GameObject> RobotList = new List<GameObject>();
    private List<Vector3> PathMap = new List<Vector3>();

    public void SaveObstacles(string _path)
    {
        IntPtr _ptr = Marshal.StringToHGlobalAnsi(_path);
        RecastMove.saveObstaclesAll(_ptr);
    }

    public void BuildPathMap()
    {
        PathMap.Clear();
        Vector3 _max = Dbgrendermesh.m_Mesh.bounds.max;
        Vector3 _min = Dbgrendermesh.m_Mesh.bounds.min;
        //
        for (float i = _min.x; i < _max.x ;)
        {
            for (float j = _min.z; j < _max.z;)
            {
                Vector3 _point = new Vector3(i,10000, j);
                RaycastHit hit;
                if (Physics.Raycast(new Ray (_point, new Vector3(0, -1, 0)), out hit, 20000.0f))
                {
                    PathMap.Add(hit.point);
                }
                j+= MapAcc;
            }
            i += MapAcc;
        }
    }

    public void StartRobot()
    {
        BuildPathMap();

        int vbsize = RecastMove.get_vb_size();
        float[] vb = new float[vbsize];
        RecastMove.get_vb(vb);
        Dbgrendermesh.Clear();
        for (int i = 0; i < vbsize;)
        {
#if UNITY_EDITOR
            Vector3 a = new Vector3(vb[i], vb[i + 1], vb[i + 2]);
            GameObject tempQie = AssetDatabase.LoadAssetAtPath("Assets/Models/obstacle_perfab/CubeOBBOX.prefab", typeof(GameObject)) as GameObject;
            GameObject tempQie2 = (GameObject)GameObject.Instantiate(tempQie);
            tempQie2.GetComponent<MeshRenderer>().enabled = true;
            tempQie2.GetComponent<BoxCollider>().enabled = false;
            tempQie2.AddComponent<RobotMove>();
            tempQie2.GetComponent<RobotMove>().PathMap = PathMap;
            tempQie2.GetComponent<RobotMove>().max = Dbgrendermesh.m_Mesh.bounds.max;
            tempQie2.GetComponent<RobotMove>().min = Dbgrendermesh.m_Mesh.bounds.min;
            //tempQie2.AddComponent<>();
            tempQie2.transform.position = a;
            tempQie2.name = i.ToString();
            RobotList.Add(tempQie2);
            i += 3;
#endif
        }
       
    }

    public void ClearAllTempObstacles()
    {
        RecastMove.clearAllTempObstacles();
    }
    public bool LoadOB(string _path)
    {
        IntPtr _ptr = Marshal.StringToHGlobalAnsi(_path);
        RecastMove.maploadob_bin(_ptr);
        RecastMove.load_ob_bin(_path);
        //for (int i = 0; i < vbsize;)
        //{
        //    Vector3 a = new Vector3(vb[i], vb[i + 1], vb[i + 2]);
        //
        //    GameObject tempQie = AssetDatabase.LoadAssetAtPath("Assets/Models/obstacle_perfab/CubeOBBOX.prefab", typeof(GameObject)) as GameObject;
        //    //GameObject tempQie = Resources.Load("perfab/CubeOBBOX") as GameObject;//通过prefab获取gameobject  
        //    GameObject tempQie2 = (GameObject)GameObject.Instantiate(tempQie);
        //    tempQie2.transform.position = a;
        //    tempQie2.name = i.ToString();
        //    i += 3;
        //}
        return false;
    }
    public bool LoadBin(string _path)
    {
        IntPtr _ptr = Marshal.StringToHGlobalAnsi(_path);
        RecastMove.mapload_bin(_ptr);
        RecastMove.load_map_bin(_path);

        //int vbsize = RecastMove.get_vb_size();
        //float[] vb = new float[vbsize]; 
        //RecastMove.get_vb(vb);
        //m_DbgRenderMesh.Clear();
        //for (int i = 0; i < vbsize;)
        //{
        //    Vector3 a = new Vector3(vb[i], vb[i + 1], vb[i + 2]);
        //    Vector3 b = new Vector3(vb[i + 3], vb[i + 4], vb[i + 5]);
        //    Vector3 c = new Vector3(vb[i + 6], vb[i + 7], vb[i + 8]);
        //
        //    m_DbgRenderMesh.AddTriangle(new DbgRenderTriangle(a, b, c, _vcolor));
        //    i += 9;
        //}
        return false;
    }

    bool first = false;
    public void ShowRecastNavmesh(DbgRenderMesh renderMesh)
    {
        renderMesh.Clear();
        int vbsize = RecastMove.get_vb_size();
        vbsize = RecastMove.get_tri_vert_count() * 3;
        float[] vb = new float[vbsize];
        //RecastMove.get_vb(vb);
        RecastMove.get_tri_vert_pos(vb);
        Dbgrendermesh.Clear();
        for (int i = 0; i < vbsize;)
        {
            Vector3 a = new Vector3(vb[i], vb[i + 1], vb[i + 2]);
            Vector3 b = new Vector3(vb[i + 3], vb[i + 4], vb[i + 5]);
            Vector3 c = new Vector3(vb[i + 6], vb[i + 7], vb[i + 8]);

            Dbgrendermesh.AddTriangle(new DbgRenderTriangle(a, b, c, _vcolor));
            i += 9;
        }

        
        renderMesh.Rebuild();
        renderMesh.ComputeBounds();
        _Bounds = new Bounds(renderMesh.m_BBCenter, renderMesh.m_Max - renderMesh.m_Min);

        int obsize = RecastMove.get_ob_count() * 3;
        float[] op = new float[obsize];
        RecastMove.get_ob_info(op);
        for(int i= 0; i != obsize;)
        {
            if (first)
                break;
            first = true;
            Vector3 pos = new Vector3(op[i++], op[i++], op[i++]);
            // radius 1.0f, height 2.0f
            GameObject tempQie = AssetDatabase.LoadAssetAtPath("Assets/Models/obstacle_perfab/CubeOBBOX.prefab", typeof(GameObject)) as GameObject;
            GameObject tempQie2 = (GameObject)GameObject.Instantiate(tempQie);
            tempQie2.transform.position = pos;
        }
    }

    void Start () {
        if (Debugrendermeshmaterial == null)
        {
            Shader vertexColor = Shader.Find("Mobile/Particles/Alpha Blended");
            Debugrendermeshmaterial = new Material(vertexColor);
        }

        Dbgrendermesh.CreateGameObject("Dbg Render Mesh", Debugrendermeshmaterial);
    }

    protected virtual void OnDisable()
    {
        RecastMove.release_explorer();
    }

    // Update is called once per frame
    void Update () {
        RecastMove.updateobstacle();
        DebugViewUpdate();
        InputUpdate();
    }

    public void InputUpdate()
    {
        bool leftClick = Input.GetMouseButtonDown(0);
        bool rightClick = Input.GetMouseButtonDown(1);
        if (leftClick || rightClick)
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray.origin, ray.direction, out hit, 1000.0f))
            {
                if (rightClick)
                {
                    print(hit.point);
                    float[] _ray = new float[3];
                    _ray[0] = ray.origin.x;
                    _ray[1] = ray.origin.y;
                    _ray[2] = ray.origin.z;
                    float[] _end = new float[3];
                    _end[0] = hit.point.x;
                    _end[1] = hit.point.y;
                    _end[2] = hit.point.z;
                    RecastMove.removeboxobstacle(_ray, _end);
                }
                else if (leftClick)
                {
                    print(hit.point);

                    float[] pmin = new float[3];
                    pmin[0] = hit.point.x - ObstaclesSize.x * 0.5f;
                    pmin[1] = hit.point.y - ObstaclesSize.y * 0.5f;
                    pmin[2] = hit.point.z - ObstaclesSize.z * 0.5f;

                    float[] pmax = new float[3];
                    pmax[0] = hit.point.x + ObstaclesSize.x * 0.5f;
                    pmax[1] = hit.point.y + ObstaclesSize.y * 0.5f;
                    pmax[2] = hit.point.z + ObstaclesSize.z * 0.5f;
                    RecastMove.addboxobstacle(pmin, pmax);
                }
            }

        }
    }


    public void DebugViewUpdate()
    {
        ShowRecastNavmesh(Dbgrendermesh);
    }


}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;
public class NavDebug : MonoBehaviour {

    private DbgRenderMesh Dbgrendermesh = new DbgRenderMesh();

    private Material Debugrendermeshmaterial = null;
    private Color _vcolor = new Color(109/255.0f,144 / 255.0f, 145 / 255.0f, 200 / 255.0f);
    public Vector3 ObstaclesSize;
    public float  MapAcc = 0.1f;

    private List<GameObject> RobotList = new List<GameObject>();
    private List<Vector3> PathMap = new List<Vector3>();


    public void ShowRecastNavmesh(DbgRenderMesh renderMesh)
    {
        renderMesh.Clear();
        int vbsize = RecastMove.get_vb_size();
        float[] vb = new float[vbsize];
        RecastMove.get_vb(vb);
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
    }

    void Start () {
       // if (Debugrendermeshmaterial == null)
       // {
       //     Shader vertexColor = Shader.Find("");
       //     Debugrendermeshmaterial = new Material(vertexColor);
       // }
       //
        Dbgrendermesh.CreateGameObject("Dbg Render Mesh", null);
    }

    protected virtual void OnDisable()
    {
        RecastMove.release_explorer();
    }

    // Update is called once per frame
    void Update () {
        RecastMove.updateobstacle();
        DebugViewUpdate();
    }

    public void DebugViewUpdate()
    {
        ShowRecastNavmesh(Dbgrendermesh);
    }
}

using System.Collections.Generic;
using UnityEngine;

public class GridManager : MonoBehaviour
{
    public Transform 水域平面;         // 拖拽场景中的水域平面
    public float 栅格尺寸 = 1f;         // 栅格尺寸（米）
    private int 栅格宽度;              // 栅格列数
    private int 栅格高度;             // 栅格行数
    private bool[,] 栅格地图;            // 栅格可通行状态（true=可通行，false=障碍物）
    private Vector3 栅格原点;         // 栅格原点（水域左下角）

    void Start()
    {
        初始化栅格();
        标记障碍物();
    }

    // 初始化栅格尺寸与数组
    private void 初始化栅格()
    {
        float 水域大小X = 水域平面.lossyScale.x * 10; // Unity 平面默认 10m×10m，缩放后计算实际大小
        float 水域大小Z = 水域平面.lossyScale.z * 10;
        栅格宽度 = Mathf.CeilToInt(水域大小X / 栅格尺寸);
        栅格高度 = Mathf.CeilToInt(水域大小Z / 栅格尺寸);
        栅格地图 = new bool[栅格宽度, 栅格高度];
        栅格原点 = 水域平面.position - new Vector3(水域大小X / 2, 0, 水域大小Z / 2);

        // 初始化为可通行
        for (int x = 0; x < 栅格宽度; x++)
        {
            for (int z = 0; z < 栅格高度; z++)
            {
                栅格地图[x, z] = true;
            }
        }
    }

    // 标记障碍物
    private void 标记障碍物()
    {
        for (int x = 0; x < 栅格宽度; x++)
        {
            for (int z = 0; z < 栅格高度; z++)
            {
                Vector3 栅格中心 = 栅格原点 + new Vector3(x * 栅格尺寸 + 栅格尺寸 / 2, 0.5f, z * 栅格尺寸 + 栅格尺寸 / 2);
                float 检测半径 = 栅格尺寸 / 2 - 0.1f;
                Collider[] 碰撞体 = Physics.OverlapSphere(栅格中心, 检测半径);

                // 标记障碍物栅格
                foreach (var 碰撞 in 碰撞体)
                {
                    if (碰撞.gameObject != 水域平面.gameObject)
                    {
                        栅格地图[x, z] = false;
                        break;
                    }
                }
            }
        }
        Debug.Log("栅格初始化完成：" + 栅格宽度 + "×" + 栅格高度 + "，障碍物已标记");
    }

    // 世界坐标转栅格坐标
    public Vector2Int 世界转栅格(Vector3 世界坐标)
    {
        Vector3 偏移 = 世界坐标 - 栅格原点;
        int x = Mathf.FloorToInt(偏移.x / 栅格尺寸);
        int z = Mathf.FloorToInt(偏移.z / 栅格尺寸);
        return new Vector2Int(Mathf.Clamp(x, 0, 栅格宽度 - 1), Mathf.Clamp(z, 0, 栅格高度 - 1));
    }

    // 栅格坐标转世界坐标
    public Vector3 栅格转世界(Vector2Int 栅格坐标)
    {
        return 栅格原点 + new Vector3(栅格坐标.x * 栅格尺寸 + 栅格尺寸 / 2, 0, 栅格坐标.y * 栅格尺寸 + 栅格尺寸 / 2);
    }

    // 检查栅格是否可通行
    public bool 栅格是否可通行(Vector2Int 栅格坐标)
    {
        if (栅格坐标.x < 0 || 栅格坐标.x >= 栅格宽度 || 栅格坐标.y < 0 || 栅格坐标.y >= 栅格高度)
            return false;
        return 栅格地图[栅格坐标.x, 栅格坐标.y];
    }

    // Gizmos 可视化栅格（调试用）
    void OnDrawGizmos()
    {
        if (栅格地图 == null || 水域平面 == null) return;

        Gizmos.color = Color.white;
        float 水域大小X = 水域平面.lossyScale.x * 10;
        float 水域大小Z = 水域平面.lossyScale.z * 10;
        栅格原点 = 水域平面.position - new Vector3(水域大小X / 2, 0, 水域大小Z / 2);

        // 绘制栅格线
        for (int x = 0; x <= 栅格宽度; x++)
        {
            Vector3 起点 = 栅格原点 + new Vector3(x * 栅格尺寸, 0, 0);
            Vector3 终点 = 栅格原点 + new Vector3(x * 栅格尺寸, 0, 栅格高度 * 栅格尺寸);
            Gizmos.DrawLine(起点, 终点);
        }
        for (int z = 0; z <= 栅格高度; z++)
        {
            Vector3 起点 = 栅格原点 + new Vector3(0, 0, z * 栅格尺寸);
            Vector3 终点 = 栅格原点 + new Vector3(栅格宽度 * 栅格尺寸, 0, z * 栅格尺寸);
            Gizmos.DrawLine(起点, 终点);
        }

        // 绘制障碍物栅格（红色）
        Gizmos.color = Color.red;
        for (int x = 0; x < 栅格宽度; x++)
        {
            for (int z = 0; z < 栅格高度; z++)
            {
                if (!栅格地图[x, z])
                {
                    Vector3 栅格中心 = 栅格原点 + new Vector3(x * 栅格尺寸 + 栅格尺寸 / 2, 0.1f, z * 栅格尺寸 + 栅格尺寸 / 2);
                    Gizmos.DrawCube(栅格中心, new Vector3(栅格尺寸 - 0.1f, 0.1f, 栅格尺寸 - 0.1f));
                }
            }
        }
    }
}
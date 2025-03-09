import React, { useCallback, useState } from "react";
import ReactFlow, {
  addEdge,
  useNodesState,
  useEdgesState,
  Controls,
  Node,
  Edge,
  Background,
  Connection,
} from "reactflow";
import "reactflow/dist/style.css";
import nodeConfigs from "../nodeConfigs";
import { GraphNode, GraphNodeData } from "../types";
import CustomNode from "./CustomNode"
import { theme } from "../theme";

const nodeTypes = {custom: CustomNode};

// Grid configuration
const GRID_SIZE = 20; // Size of each grid cell in pixels
const SNAP_GRID: [number, number] = [GRID_SIZE, GRID_SIZE];

const GraphEditor: React.FC = () => {
  const [nodes, setNodes, onNodesChange] = useNodesState<GraphNodeData>([]);
  const [edges, setEdges, onEdgesChange] = useEdgesState<Edge[]>([]);
  const [selectedNode, setSelectedNode] = useState<GraphNode | null>(null);

  const onConnect = useCallback(
    (params: Connection) => setEdges((eds) => addEdge(params, eds)),
    []
  );

  const onEdgeContextMenu: EdgeMouseHandler = useCallback(
    (event: MouseEvent, edge: Edge) => {
      event.preventDefault();
      setEdges((eds) => eds.filter((e) => e.id !== edge.id));
    },
    [setEdges]
  );

  const onNodeContextMenu: NodeMouseHandler = useCallback(
    (event: MouseEvent, node: Node) => {
      event.preventDefault();
      // Remove the node
      setNodes((nds) => nds.filter((n) => n.id !== node.id));
      // Remove any edges connected to this node
      setEdges((eds) => eds.filter((e) => e.source !== node.id && e.target !== node.id));
    },
    [setNodes, setEdges]
  );

  const onDrop = useCallback(
    (event: React.DragEvent<HTMLDivElement>) => {
      event.preventDefault();
      const nodeData = JSON.parse(event.dataTransfer.getData("application/reactflow"));
      if (!nodeData) return;

      // Snap the initial position to grid
      const x = Math.round((event.clientX - 250) / GRID_SIZE) * GRID_SIZE;
      const y = Math.round((event.clientY - 50) / GRID_SIZE) * GRID_SIZE;
      const position = { x, y };
      const config = nodeConfigs[nodeData.id] || { name: nodeData.label, config: {} };

      const newNode: Node<GraphNodeData> = {
        id: `${nodeData.id}-${nodes.length}`,
        type: "custom",
        position,
        data: { label: config.name, config: config.config },
      };

      setNodes((nds) => [...nds, newNode]);
    },
    [setNodes, nodes]
  );

  return (
    <div style={{ 
      display: "flex", 
      height: "100vh", 
      width: "100%",
      fontFamily: theme.fonts.primary,
      background: theme.colors.bg,
      color: theme.colors.primary 
    }}>
      <div style={{ flexGrow: 1, height: "100vh" }} onDrop={onDrop} onDragOver={(event) => event.preventDefault()}>
        <ReactFlow
          nodes={nodes}
          edges={edges}
          nodeTypes={nodeTypes}
          onNodesChange={onNodesChange}
          onEdgesChange={onEdgesChange}
          onConnect={onConnect}
          onEdgeContextMenu={onEdgeContextMenu}
          onNodeContextMenu={onNodeContextMenu}
          snapToGrid={true}
          snapGrid={SNAP_GRID}
          style={{
            backgroundColor: theme.colors.bg,
            width: "100%",
            height: "100%",
          }}
          defaultEdgeOptions={{
            style: { 
              stroke: theme.colors.primary, 
              strokeWidth: 2,
            },
            animated: false,
            type: 'default',
          }}
          fitView
        >
          <Background 
            gap={GRID_SIZE} 
            size={1} 
            color={theme.colors.gridColor}
          />
          <Controls 
            style={{
              backgroundColor: theme.colors.controlsBg,
              color: theme.colors.controlsColor,
              borderColor: theme.colors.nodeBorder,
            }}
          />
        </ReactFlow>
      </div>
    </div>
  );
};

export default GraphEditor;


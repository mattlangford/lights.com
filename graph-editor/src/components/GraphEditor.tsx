import React, { useCallback, useState, MouseEvent, DragEvent, useRef } from "react";
import ReactFlow, {
  addEdge,
  useNodesState,
  useEdgesState,
  Controls,
  Node,
  Edge,
  Background,
  Connection,
  ReactFlowInstance,
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
  const [reactFlowInstance, setReactFlowInstance] = useState<ReactFlowInstance | null>(null);
  const reactFlowWrapper = useRef<HTMLDivElement>(null);

  const onConnect = useCallback(
    (params: Connection) => setEdges((eds) => addEdge(params, eds)),
    []
  );

  const onEdgeContextMenu = useCallback(
    (event: MouseEvent, edge: Edge) => {
      event.preventDefault();
      setEdges((eds) => eds.filter((e) => e.id !== edge.id));
    },
    [setEdges]
  );

  const onNodeContextMenu = useCallback(
    (event: MouseEvent, node: Node) => {
      event.preventDefault();
      // Remove the node
      setNodes((nds) => nds.filter((n) => n.id !== node.id));
      // Remove any edges connected to this node
      setEdges((eds) => eds.filter((e) => e.source !== node.id && e.target !== node.id));
    },
    [setNodes, setEdges]
  );

  const onDragOver = useCallback((event: DragEvent<HTMLDivElement>) => {
    event.preventDefault();
    event.dataTransfer.dropEffect = 'move';
  }, []);

  const onDrop = useCallback(
    (event: DragEvent<HTMLDivElement>) => {
      event.preventDefault();

      if (!reactFlowInstance || !reactFlowWrapper.current) return;

      const nodeData = JSON.parse(event.dataTransfer.getData("application/reactflow"));
      if (!nodeData) return;

      // Get the position relative to the ReactFlow wrapper
      const reactFlowBounds = reactFlowWrapper.current.getBoundingClientRect();
      
      // Calculate the position where the node should be placed
      const position = reactFlowInstance.project({
        x: event.clientX - reactFlowBounds.left,
        y: event.clientY - reactFlowBounds.top,
      });

      // Snap to grid
      const x = Math.round(position.x / GRID_SIZE) * GRID_SIZE;
      const y = Math.round(position.y / GRID_SIZE) * GRID_SIZE;
      
      const config = nodeConfigs[nodeData.id] || { name: nodeData.label, config: {} };

      const newNode: Node<GraphNodeData> = {
        id: `${nodeData.id}-${nodes.length}`,
        type: "custom",
        position: { x, y },
        data: { label: config.name, config: config.config },
      };

      setNodes((nds) => [...nds, newNode]);
    },
    [reactFlowInstance, setNodes, nodes]
  );

  return (
    <div style={{ 
      display: "flex", 
      height: "100vh",
      width: "100vw",
      margin: 0,
      padding: 0,
      overflow: "hidden",
      fontFamily: theme.fonts.primary,
      background: theme.colors.bg,
      color: theme.colors.primary 
    }}>
      <div 
        ref={reactFlowWrapper}
        style={{ 
          flexGrow: 1, 
          height: "100%",
          margin: 0,
          padding: 0,
          position: "relative",
        }} 
      >
        <ReactFlow
          nodes={nodes}
          edges={edges}
          nodeTypes={nodeTypes}
          onNodesChange={onNodesChange}
          onEdgesChange={onEdgesChange}
          onConnect={onConnect}
          onEdgeContextMenu={onEdgeContextMenu}
          onNodeContextMenu={onNodeContextMenu}
          onInit={setReactFlowInstance}
          onDrop={onDrop}
          onDragOver={onDragOver}
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
          defaultViewport={{ x: 0, y: 0, zoom: 1 }}
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


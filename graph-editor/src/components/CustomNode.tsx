import React from "react";
import { Handle, Position } from "reactflow";
import { theme } from "../theme";

interface CustomNodeProps {
  data: { label: string; config: Record<string, number | boolean | string> };
}

const CustomNode: React.FC<CustomNodeProps> = ({ data }) => {
  const inputs = typeof data.config.inputs === 'number' ? data.config.inputs : 0;
  const outputs = typeof data.config.outputs === 'number' ? data.config.outputs : 0;

  return (
    <div
      style={{
        padding: theme.spacing.nodePadding,
        borderRadius: theme.spacing.borderRadius,
        background: theme.colors.nodeBg,
        border: `1px solid ${theme.colors.nodeBorder}`,
        textAlign: "center",
        minWidth: "150px",
        position: "relative",
        color: theme.colors.nodeColor,
        boxShadow: theme.shadows.node,
      }}
    >
      {/* Render Inputs on the Left */}
      {Array.from({ length: Number(inputs) }).map((_, index) => (
        <Handle
          key={`input-${index}`}
          type="target"
          position={Position.Left}
          style={{ 
            top: `${(index + 1) * (100 / (Number(inputs) + 1))}%`, 
            background: theme.colors.handleBg,
            border: `2px solid ${theme.colors.handleBorder}`
          }}
        />
      ))}

      <div>{data.label}</div>

      {/* Render Outputs on the Right */}
      {Array.from({ length: Number(outputs) }).map((_, index) => (
        <Handle
          key={`output-${index}`}
          type="source"
          position={Position.Right}
          style={{ 
            top: `${(index + 1) * (100 / (Number(outputs) + 1))}%`, 
            background: theme.colors.handleBg,
            border: `2px solid ${theme.colors.handleBorder}`
          }}
        />
      ))}
    </div>
  );
};

export default CustomNode;

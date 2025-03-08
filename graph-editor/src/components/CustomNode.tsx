import React from "react";
import { Handle, Position } from "reactflow";

interface CustomNodeProps {
  data: { label: string; config: Record<string, number | boolean | string> };
}

const CustomNode: React.FC<CustomNodeProps> = ({ data }) => {
  const inputs = data.config.inputs || 0;
  const outputs = data.config.outputs || 0;

  return (
    <div
      style={{
        padding: "10px",
        borderRadius: "5px",
        background: "#ddd",
        border: "1px solid #aaa",
        textAlign: "center",
        minWidth: "150px",
        position: "relative",
      }}
    >
      {/* Render Inputs on the Left */}
      {Array.from({ length: inputs }).map((_, index) => (
        <Handle
          key={`input-${index}`}
          type="target"
          position={Position.Left}
          style={{ top: `${(index + 1) * (100 / (inputs + 1))}%`, background: "#555" }}
        />
      ))}

      <div>{data.label}</div>

      {/* Render Outputs on the Right */}
      {Array.from({ length: outputs }).map((_, index) => (
        <Handle
          key={`output-${index}`}
          type="source"
          position={Position.Right}
          style={{ top: `${(index + 1) * (100 / (outputs + 1))}%`, background: "#555" }}
        />
      ))}
    </div>
  );
};

export default CustomNode;

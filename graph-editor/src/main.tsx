import React from "react";
import ReactDOM from "react-dom/client";
import Sidebar from "./components/Sidebar";
import GraphEditor from "./components/GraphEditor";

const App: React.FC = () => {
  return (
    <div style={{ display: "flex", height: "100vh", width: "100%" }}>
      <Sidebar />
      <GraphEditor />
    </div>
  );
};

ReactDOM.createRoot(document.getElementById("root")!).render(<App />);

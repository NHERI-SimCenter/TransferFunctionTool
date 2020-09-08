function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut", "@TargetDir@/TransferFunctionTool.exe", "@StartMenuDir@/Transfer Function Tool.lnk",
            "workingDirectory=@TargetDir@", "description=Start SimCenter Transfer Function Tool");
    }
}

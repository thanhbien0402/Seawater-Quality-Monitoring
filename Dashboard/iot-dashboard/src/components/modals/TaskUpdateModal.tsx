import {
    Dialog,
    DialogContent,
    DialogDescription,
    DialogHeader,
    DialogTitle,
} from "@/components/ui/dialog"
import { Dispatch, SetStateAction } from "react"
import { TaskUpdateForm } from "../forms/TaskUpdateForm"

type TData = {
    id: string
    task_type: string
    task_date: string
    task_time: string
    trigger_type: string
    status: string
    createdAt: string
}

export const TaskUpdateModal = ({ isOpen, isSetOpen, updatedData, taskId, userId, loadTask, deviceId, rpcID }: { isOpen: boolean, isSetOpen: Dispatch<SetStateAction<boolean>>, updatedData: TData, taskId: string, userId: string, loadTask: any, deviceId: string, rpcID: number }) => {
    return (
        <Dialog open={isOpen} onOpenChange={isSetOpen}>
            <DialogContent>
                <DialogHeader>
                    <DialogTitle>Update Task</DialogTitle>
                </DialogHeader>
                <TaskUpdateForm updatedData={updatedData} taskId={taskId} userId={userId} loadTask={loadTask} deviceId={deviceId} rpcID={rpcID} />
            </DialogContent>
        </Dialog>
    )
}

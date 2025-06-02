"use client"

import { ColumnDef } from "@tanstack/react-table"
import { MoreHorizontal } from "lucide-react"
import { ArrowUpDown } from "lucide-react"
import { useToast } from '@/hooks/use-toast'

import { Button } from "@/components/ui/button"
import {
    DropdownMenu,
    DropdownMenuContent,
    DropdownMenuItem,
    DropdownMenuLabel,
    DropdownMenuSeparator,
    DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu"

import {
    AlertDialog,
    AlertDialogAction,
    AlertDialogCancel,
    AlertDialogContent,
    AlertDialogDescription,
    AlertDialogFooter,
    AlertDialogHeader,
    AlertDialogTitle,
    AlertDialogTrigger,
} from "@/components/ui/alert-dialog"

import { Checkbox } from "@/components/ui/checkbox"
import { useGetUserSB } from "@/hooks/useGetUserSB"
import { TaskUpdateModal } from "@/components/modals/TaskUpdateModal"
import { useState } from "react"
import { deleteTask, sendDataTb } from "@/actions"
import { Badge } from "@/components/ui/badge"

// This type is used to define the shape of our data.
// You can use a Zod schema here if you want.

export type Notification = {
    id: string
    createdAt: string
    deviceId: string
    deviceName: string
    sensor: string
    condition: string
    content: string
}

export type Task = {
    id: string
    task_type: string
    task_date: string
    task_time: string
    trigger_type: string
    status: string,
    rpcID: number,
    createdAt: string
}

export const getColumns = (loadNotification: any): ColumnDef<Notification>[] => [
    {
        id: "select",
        header: ({ table }) => (
            <Checkbox
                checked={
                    table.getIsAllPageRowsSelected() ||
                    (table.getIsSomePageRowsSelected() && "indeterminate")
                }
                onCheckedChange={(value: any) => table.toggleAllPageRowsSelected(!!value)}
                aria-label="Select all"
            />
        ),
        cell: ({ row }) => (
            <Checkbox
                checked={row.getIsSelected()}
                onCheckedChange={(value: any) => row.toggleSelected(!!value)}
                aria-label="Select row"
            />
        ),
        enableSorting: false,
        enableHiding: false,
    },
    {
        accessorKey: "createdAt",
        header: ({ column }) => {
            return (
                <Button
                    variant="ghost"
                    onClick={() => column.toggleSorting(column.getIsSorted() === "asc")}
                >
                    Created At
                    <ArrowUpDown className="ml-2 h-4 w-4" />
                </Button>
            )
        },
    },
    {
        accessorKey: "deviceName",
        header: ({ column }) => {
            return (
                <Button
                    variant="ghost"
                    onClick={() => column.toggleSorting(column.getIsSorted() === "asc")}
                >
                    Device Name
                    <ArrowUpDown className="ml-2 h-4 w-4" />
                </Button>
            )
        },
    },
    {
        accessorKey: "sensor",
        header: ({ column }) => {
            return (
                <Button
                    variant="ghost"
                    onClick={() => column.toggleSorting(column.getIsSorted() === "asc")}
                >
                    Sensor Type
                    <ArrowUpDown className="ml-2 h-4 w-4" />
                </Button>
            )
        },
    },
    {
        accessorKey: "condition",
        header: ({ column }) => {
            return (
                <Button
                    variant="ghost"
                    onClick={() => column.toggleSorting(column.getIsSorted() === "asc")}
                >
                    Notification Condition
                    <ArrowUpDown className="ml-2 h-4 w-4" />
                </Button>
            )
        },
        cell: ({ getValue }) => {
            const v = getValue<string>()
            return <Badge variant='destructive'>{v}</Badge>
        }
    },
    {
        accessorKey: "content",
        header: "Status",
    },
    {
        id: "actions",
        cell: ({ row }) => {
            const task = row.original
            // const { id, task_type, rpcID } = task
            const res = useGetUserSB()
            const userId = res.userInfo?.session.user.id

            const [isOpen, setIsOpen] = useState<boolean>(false)
            const [isDelete, setIsDelete] = useState<boolean>(false)

            const { toast } = useToast()

            return (
                <>
                    <DropdownMenu>
                        <DropdownMenuTrigger asChild>
                            <Button variant="ghost" className="h-8 w-8 p-0">
                                <span className="sr-only">Open menu</span>
                                <MoreHorizontal className="h-4 w-4" />
                            </Button>
                        </DropdownMenuTrigger>
                        <DropdownMenuContent align="end">
                            <DropdownMenuLabel>Actions</DropdownMenuLabel>
                            <DropdownMenuSeparator />
                            <DropdownMenuItem className="hover:!text-yellow-600" onClick={() => {
                                setIsOpen(true)
                            }}>
                                Update task
                            </DropdownMenuItem>
                            <DropdownMenuItem className="hover:!text-red-600" onClick={() => {
                                setIsDelete(true)
                            }}>
                                Delete Task
                            </DropdownMenuItem>
                        </DropdownMenuContent>
                    </DropdownMenu>
                    {/* <TaskUpdateModal
                        isOpen={isOpen}
                        isSetOpen={setIsOpen}
                        updatedData={task}
                        taskId={id}
                        userId={userId}
                        loadTask={loadTask}
                        deviceId={deviceId}
                        rpcID={rpcID}
                    />
                    <AlertDialog open={isDelete} onOpenChange={setIsDelete}>
                        <AlertDialogContent>
                            <AlertDialogHeader>
                                <AlertDialogTitle>Are you absolutely sure?</AlertDialogTitle>
                                <AlertDialogDescription>
                                    This action cannot be undone. This will permanently delete your
                                    account and remove your data from our servers.
                                </AlertDialogDescription>
                            </AlertDialogHeader>
                            <AlertDialogFooter>
                                <AlertDialogCancel>Cancel</AlertDialogCancel>
                                <AlertDialogAction onClick={async () => {
                                    const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))

                                    await deleteTask(id, userId)
                                    await sendDataTb(task_type, 'delete', token, deviceId, rpcID)
                                    loadTask()
                                    toast({
                                        title: "You submitted the following values:",
                                        description: (
                                            <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
                                                <p className="text-white">Deleted Successfully</p>
                                            </pre>
                                        ),
                                    })
                                }}>Continue</AlertDialogAction>
                            </AlertDialogFooter>
                        </AlertDialogContent>
                    </AlertDialog> */}
                </>
            )
        },
    },
]

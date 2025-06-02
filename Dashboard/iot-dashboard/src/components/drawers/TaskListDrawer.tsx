'use client'
import {
    Drawer,
    DrawerClose,
    DrawerContent,
    DrawerDescription,
    DrawerFooter,
    DrawerHeader,
    DrawerTitle,
    DrawerTrigger
} from '../ui/drawer'
import { Button } from '../ui/button'
import { ClipboardList } from 'lucide-react'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import TaskTable from '@/app/tasks/page'
import { TaskForm } from '../forms/TaskForm'
import { useState } from 'react'


export function TaskListDrawer({ deviceId }: { deviceId: string }) {

    const [isOpen, setIsOpen] = useState<boolean>(false)

    return (
        <Drawer open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DrawerTrigger asChild>
                <Button variant="ghost">
                    <ClipboardList className="h-4 w-4" />
                </Button>
            </DrawerTrigger>
            <DrawerContent>
                <div className="w-full h-[75vh] space-y p-3 rounded-md border-4 border-black">
                    <Tabs defaultValue="create" className="w-full">
                        <TabsList className="grid w-full grid-cols-2">
                            <TabsTrigger value="create">Create Task</TabsTrigger>
                            <TabsTrigger value="view">View Task</TabsTrigger>
                        </TabsList>
                        <TabsContent value="create">
                            {/* <SignInForm /> */}
                            <TaskForm setIsOpen={setIsOpen} deviceId={deviceId} />
                        </TabsContent>
                        <TabsContent value="view">
                            {/* <RegisterForm /> */}
                            <DrawerHeader className="text-left">
                                <DrawerTitle>Task List</DrawerTitle>
                                <DrawerDescription>
                                    View your task list below.
                                </DrawerDescription>
                            </DrawerHeader>
                            <TaskTable deviceId={deviceId} />
                        </TabsContent>
                    </Tabs>
                </div>
                <DrawerFooter className="pt-2">
                    <DrawerClose asChild>
                        <Button type="button" variant="default">
                            Close
                        </Button>
                    </DrawerClose>
                </DrawerFooter>
            </DrawerContent>
        </Drawer>
    )
}